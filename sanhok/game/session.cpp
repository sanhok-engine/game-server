#include <sanhok/game/session.hpp>
#include <sanhok/game/protocol/protocol.hpp>

namespace sanhok::game {
Session::Session(const SessionID id, const unsigned short listen_port)
    : id(id), listener_(ctx_, listen_port, get_listener_on_acceptance()), game_manager_(1) {}

Session::~Session() {
    stop();
    if (worker_thread_.joinable()) worker_thread_.join();
    if (io_thread_.joinable()) io_thread_.join();
}

void Session::start() {
    if (is_running_.exchange(true)) return;
    is_listening_ = true;
    listener_.start();

    worker_thread_ = std::thread([this] {
        auto last_tick_time = steady_clock::now();
        while (is_running_) {
            const auto current_time = steady_clock::now();
            const auto elapsed_time = duration_cast<milliseconds>(current_time - last_tick_time);
            if (elapsed_time < TICK_INTERVAL) continue;

            update(elapsed_time);
            last_tick_time = current_time;
        }
    });
    worker_thread_.detach();

    io_thread_ = std::thread([this] {
        while (is_running_) {
            ctx_.run();
        }
    });
    io_thread_.detach();
}

void Session::stop() {
    if (!is_running_.exchange(false)) return;
    is_listening_ = false;

    game_manager_.stop();
    listener_.stop();
    clients_.clear();
}

std::function<void(boost::asio::io_context&, tcp::socket&&)> Session::get_listener_on_acceptance() {
    return [this](boost::asio::io_context& ctx, tcp::socket&& socket) {
        static std::atomic<ClientID> id_generator {0};

        const auto new_id = ++id_generator;
        client_join(std::make_shared<Client>(ctx, new_id, std::move(socket)));
    };
}

void Session::client_join(std::shared_ptr<Client> client) {
    if (!is_listening_) {
        spdlog::warn("[Session] Client ({}) is denied", client->id);
        client->stop();
        return;
    }

    const auto client_id = client->id;
    spdlog::info("[Session] Client ({}) has joined", client_id);
    client->start();

    // Send ClientJoin message to the new client
    using namespace protocol;
    flatbuffers::FlatBufferBuilder builder {64};
    const auto client_join = CreateClientJoin(builder, client_id, client->local_endpoint_udp().port());
    builder.FinishSizePrefixed(CreateProtocol(builder, ProtocolType::ClientJoin, client_join.Union()));
    client->send_tcp(std::make_shared<flatbuffers::DetachedBuffer>(builder.Release()));

    clients_.insert_or_assign(client_id, std::move(client));
}

void Session::client_leave(const ClientID client_id) {
    game_manager_.player_leave(client_id);
    clients_.erase(client_id);
}

void Session::update(const milliseconds dt) {
    clients_.apply_all([dt](std::shared_ptr<Client>& client) {
        client->update(dt);
    });
}
}

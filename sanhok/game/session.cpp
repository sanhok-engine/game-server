#include <sanhok/game/session.hpp>

namespace sanhok::game {
Session::Session(const SessionID id, const unsigned short listen_port, const size_t zones)
    : id(id), listener_(ctx_, listen_port, get_listener_on_acceptance()), zones_(zones) {}

Session::~Session() {
    stop();
    if (worker_thread_.joinable()) worker_thread_.join();
    if (io_thread_.joinable()) io_thread_.join();
}

void Session::start() {
    if (is_running_.exchange(true)) return;

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
}

void Session::stop() {
    if (!is_running_.exchange(false)) return;

    listener_.stop();
}

std::function<void(boost::asio::io_context&, tcp::socket&&)> Session::get_listener_on_acceptance() {
    return [this](boost::asio::io_context& ctx, tcp::socket&& socket) {
        static std::atomic<ClientID> id_generator {0};

        if (state_ != State::PREPARING) {
            try { socket.close(); } catch (const boost::system::system_error& e) {}
            return;
        }

        const auto new_id = ++id_generator;
        zone_default_.clients.insert_or_assign(new_id, std::make_shared<Client>(ctx, std::move(socket), new_id));
    };
}

bool Session::client_join(std::shared_ptr<Client> client) {
    if (state_ != State::PREPARING) return false;

    const auto client_id = client->id;
    zone_default_.clients.insert_or_assign(client_id, std::move(client));
    return true;
}

void Session::client_leave(const ClientID client_id) {
    if (zone_default_.clients.contains(client_id)) {
        zone_default_.clients.erase(client_id);
    }

    for (auto& zone : zones_) {
        if (!zone.clients.contains(client_id)) continue;
        zone.clients.erase(client_id);
    }
}

void Session::update(milliseconds dt) {}
}

#include <sanhok/game/dummy/dummy_client.hpp>

namespace sanhok::game::dummy {
DummyClient::DummyClient()
    : peer_tcp_(ctx_, tcp::socket {ctx_}, get_protocol_handler(false)),
    peer_udp_(ctx_, udp::endpoint(udp::v4(), 0), get_protocol_handler(true)) {}

DummyClient::~DummyClient() {
    stop();
}

void DummyClient::start(tcp::endpoint&& remote_endpoint) {
    if (is_running_.exchange(true)) return;

    co_spawn(ctx_, [this, remote_endpoint = std::move(remote_endpoint)]()->boost::asio::awaitable<void> {
        if (!co_await peer_tcp_.connect(remote_endpoint)) {
            spdlog::error("[DummyClient] Error connecting server");
            stop();
            co_return;
        }
        peer_tcp_.run();

        // peer_udp_.connect(udp::endpoint(remote_endpoint.address(), UDP_PORT));
        // peer_udp_.open();
    }, boost::asio::detached);

    worker_thread_ = std::thread([this] {
        while (is_running_) {
            auto last_tick_time = steady_clock::now();
            while (is_running_) {
                const auto current_time = steady_clock::now();
                const auto elapsed_time = duration_cast<milliseconds>(current_time - last_tick_time);
                if (elapsed_time < TICK_INTERVAL) continue;

                update(elapsed_time);
                last_tick_time = current_time;
            }
        }
    });
    worker_thread_.detach();

    ctx_.run();
}

void DummyClient::stop() {
    if (!is_running_.exchange(false)) return;

    peer_tcp_.disconnect();
    peer_udp_.close();

    if (worker_thread_.joinable()) worker_thread_.join();
}

void DummyClient::update(milliseconds dt) {
    if (!is_joined_) return;
}

std::function<void(std::vector<uint8_t>&&)> DummyClient::get_protocol_handler(const bool buffer_size_prefixed) {
    return [this, buffer_size_prefixed](std::vector<uint8_t>&& buffer)->void {
        using namespace sanhok::game::protocol;

        const auto protocol = buffer_size_prefixed
            ? GetSizePrefixedProtocol(buffer.data())
            : GetProtocol(buffer.data());
        if (!protocol) {
            spdlog::error("[GameClient] Invalid protocol structure");
            stop();
            return;
        }

        switch (protocol->protocol_type()) {
        case ProtocolType::ClientJoin:
            handle_client_join(protocol->protocol_as<ClientJoin>());
        default:
            break;
        }
    };
}

void DummyClient::handle_client_join(const protocol::ClientJoin* client_join) {
    id_ = client_join->client_id();
    spdlog::info("[DummyClient] Joined the server with id ({})", id_);

    is_joined_ = true;
}
}

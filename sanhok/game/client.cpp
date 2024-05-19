#include <sanhok/game/client.hpp>
#include <sanhok/game/protocol/protocol.hpp>
#include <spdlog/spdlog.h>

namespace sanhok::game {
Client::Client(boost::asio::io_context& ctx, tcp::socket&& socket, const ClientID id)
    : id(id), ctx_(ctx),
    peer_tcp_(ctx_, std::move(socket), get_protocol_handler(false)),
    peer_udp_(ctx_, udp::endpoint(udp::v4(), UDP_PORT), get_protocol_handler(true)) {
    peer_udp_.connect(udp::endpoint(peer_tcp_.remote_endpoint().address(), UDP_PORT));
}

Client::~Client() {
    stop();
}

void Client::start() {
    if (is_running_.exchange(true)) return;

    peer_tcp_.run();
    peer_udp_.open();
}

void Client::stop() {
    if (!is_running_.exchange(false)) return;

    peer_tcp_.disconnect();
    peer_udp_.close();
}

void Client::send_tcp(std::shared_ptr<flatbuffers::DetachedBuffer> buffer) {
    peer_tcp_.send_message(std::move(buffer));
}

void Client::send_udp(std::shared_ptr<flatbuffers::DetachedBuffer> buffer) {
    peer_udp_.send_packet(std::move(buffer));
}

std::function<void(std::vector<uint8_t>&&)> Client::get_protocol_handler(const bool buffer_size_prefixed) {
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
        case ProtocolType::PlayerMovement:
            handle_protocol_player_movement(protocol->protocol_as<PlayerMovement>());
        default:
            break;
        }
    };
}

void Client::handle_protocol_player_movement(const protocol::PlayerMovement* player_movement) {
    if (!player_movement) return;
}
}

#include <sanhok/game/game_client.hpp>
#include <sanhok/game/protocol/protocol.hpp>
#include <spdlog/spdlog.h>

namespace sanhok::game {
GameClient::GameClient(boost::asio::io_context& ctx, tcp::socket&& socket, const ClientID id)
    : ctx_(ctx),
    peer_tcp_(ctx_, std::move(socket), get_protocol_handler(false)),
    peer_udp_(ctx_, udp::endpoint(udp::v4(), UDP_PORT),
        udp::endpoint(peer_tcp_.remote_endpoint().address(), UDP_PORT), get_protocol_handler(true)) {}

void GameClient::start() {
    if (is_running_.exchange(true)) return;

    peer_tcp_.run();
    peer_udp_.open();
}

void GameClient::stop() {
    if (!is_running_.exchange(false)) return;

    peer_tcp_.disconnect();
    peer_udp_.close();
}

std::function<void(std::vector<uint8_t>&&)> GameClient::get_protocol_handler(const bool buffer_size_prefixed) {
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

        const flatbuffers::Verifier verifier {buffer.data(), buffer.size()};
        switch (protocol->protocol_type()) {
        case ProtocolType::PlayerMovement:
            handle_protocol_player_movement(protocol->protocol_as<PlayerMovement>());
        default:
            break;
        }
    };
}

void GameClient::handle_protocol_player_movement(const protocol::PlayerMovement* player_movement) {
    if (!player_movement) return;
}
}

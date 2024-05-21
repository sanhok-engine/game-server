#include <sanhok/game/client.hpp>
#include <sanhok/game/protocol/protocol.hpp>
#include <spdlog/spdlog.h>

namespace sanhok::game {
Client::Client(boost::asio::io_context& ctx, const ClientID id, tcp::socket&& socket)
    : id(id), ctx_(ctx),
    peer_tcp_(ctx_, std::move(socket),
        get_on_connection(), get_on_disconnection(), get_protocol_handler(false)),
    peer_udp_(ctx_, udp::endpoint(udp::v4(), 0), get_protocol_handler(true)) {
    peer_tcp_.set_no_delay(true);
}

Client::~Client() {
    stop();
}

void Client::start() {
    if (is_running_.exchange(true)) return;

    peer_tcp_.run();
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

void Client::update(const milliseconds dt) {
    if (!is_running_) return;

    player.movement.move(dt);
}

std::function<void()> Client::get_on_connection() {
    return [this] {};
}

std::function<void()> Client::get_on_disconnection() {
    return [this] {
        stop();
    };
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

        // flatbuffers::Verifier verifier {buffer.data(), buffer.size()};
        switch (protocol->protocol_type()) {
            [[unlikely]] case ProtocolType::ClientJoin:
            handle_protocol_client_join(protocol->protocol_as<ClientJoin>());
            break;
            [[likely]] case ProtocolType::PlayerMovement:
            handle_protocol_player_movement(protocol->protocol_as<PlayerMovement>());
            break;
        default:
            break;
        }
    };
}

void Client::handle_protocol_client_join(const protocol::ClientJoin* client_join) {
    if (!client_join) return;
    if (id != client_join->client_id()) return;

    peer_udp_.connect(udp::endpoint(peer_tcp_.remote_endpoint().address(), client_join->udp_port()));
    peer_udp_.open();
}

void Client::handle_protocol_player_movement(const protocol::PlayerMovement* player_movement) {
    if (!player_movement) return;
    if (id != player_movement->client_id()) return;

    //TODO: Ignore older packet?
    const auto body_direction = player_movement->body_diection();
    const auto aim_direction = player_movement->aim_direction();
    player.movement.body_direction = {body_direction->x(), body_direction->y(), body_direction->z()};
    player.movement.aim_direction = {aim_direction->x(), aim_direction->y(), aim_direction->z()};
    player.movement.movement_type = player_movement->movement_type();
    player.movement.movement_direction = player_movement->movement_direction();
}
}

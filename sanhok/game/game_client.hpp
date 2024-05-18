#pragma once

#include <sanhok/game/protocol/player_movement.hpp>
#include <sanhok/net/peer_tcp.hpp>
#include <sanhok/net/peer_udp.hpp>

namespace sanhok::game {
using boost::asio::ip::tcp;
using boost::asio::ip::udp;
using ClientID = uint32_t;

class GameClient final {
    constexpr static unsigned short UDP_PORT = 50010;

public:
    GameClient(boost::asio::io_context& ctx, tcp::socket&& socket, ClientID id);

    void start();
    void stop();

    bool is_running() const { return is_running_; }

private:
    std::function<void(std::vector<uint8_t>&&)> get_protocol_handler(bool buffer_size_prefixed);
    void handle_protocol_player_movement(const protocol::PlayerMovement* player_movement);

    boost::asio::io_context& ctx_;
    net::PeerTCP peer_tcp_;
    net::PeerUDP peer_udp_;

    std::atomic<bool> is_running_ {false};
};
}

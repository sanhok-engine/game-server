#pragma once

#include <boost/asio.hpp>
#include <sanhok/concurrent_map.hpp>
#include <sanhok/game/game_client.hpp>
#include <sanhok/net/listener_tcp.hpp>

#include <chrono>

namespace sanhok::game {
using namespace std::chrono;

class GameServer final {
    //TODO: Extract options to option file
    constexpr static unsigned short LISTEN_PORT = 50000;
    constexpr static milliseconds TICK_INTERVAL = 33ms; // 30 Hz

public:
    GameServer();
    ~GameServer();

    void start();
    void stop();

    bool is_running() const { return is_running_; }

private:
    void update(milliseconds dt);

    boost::asio::io_context ctx_ {};
    net::ListenerTCP listener_;
    ConcurrentMap<ClientID, std::shared_ptr<GameClient>> clients_ {};

    std::thread worker_;
    std::atomic<bool> is_running_ {false};
};
}

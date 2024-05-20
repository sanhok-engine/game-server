#pragma once

#include <boost/asio.hpp>
#include <sanhok/concurrent_map.hpp>
#include <sanhok/game/client.hpp>
#include <sanhok/game/game_manager.hpp>
#include <sanhok/net/listener_tcp.hpp>

#include <chrono>

namespace sanhok::game {
using namespace std::chrono;
using SessionID = uint32_t;

class Session {
    //TODO: Extract options to option file
    constexpr static milliseconds TICK_INTERVAL = 33ms; // 30 Hz

public:
    Session(SessionID id, unsigned short listen_port);
    ~Session();

    void start();
    void stop();
    void client_join(std::shared_ptr<Client> client);
    void client_leave(ClientID client_id);

    bool is_running() const { return is_running_; }
    bool is_listening() const { return is_listening_; }
    tcp::endpoint listen_endpoint() const { return listener_.local_endpoint(); }

    std::function<void(boost::asio::io_context&, tcp::socket&&)> get_listener_on_acceptance();

private:
    void update(milliseconds dt);

public:
    const SessionID id;

private:
    boost::asio::io_context ctx_ {};
    net::ListenerTCP listener_;
    std::atomic<bool> is_running_ {false};
    std::atomic<bool> is_listening_ {false};

    ConcurrentMap<ClientID, std::shared_ptr<Client>> clients_ {};
    GameManager game_manager_;

    std::thread worker_thread_;
    std::thread io_thread_;
};
}

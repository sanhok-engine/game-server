#pragma once

#include <boost/asio.hpp>
#include <sanhok/concurrent_map.hpp>
#include <sanhok/game/client.hpp>
#include <sanhok/game/zone.hpp>
#include <sanhok/net/listener_tcp.hpp>

#include <chrono>

namespace sanhok::game {
using namespace std::chrono;
using SessionID = uint32_t;

class Session {
    //TODO: Extract options to option file
    constexpr static milliseconds TICK_INTERVAL = 33ms; // 30 Hz

public:
    enum class State { PREPARING, PLAYING, FINISHED };

    Session(SessionID id, unsigned short listen_port, size_t zones);
    ~Session();

    void start();
    void stop();
    void client_join(std::shared_ptr<Client> client);
    void client_leave(ClientID client_id);

    bool is_running() const { return is_running_; }
    State state() const { return state_; }
    std::function<void(boost::asio::io_context&, tcp::socket&&)> get_listener_on_acceptance();

private:
    void update(milliseconds dt);

public:
    const SessionID id;

private:
    boost::asio::io_context ctx_ {};
    net::ListenerTCP listener_;
    std::atomic<bool> is_running_ {false};

    State state_ {State::PREPARING};
    ConcurrentMap<ClientID, std::shared_ptr<Client>> clients_ {};
    std::vector<Zone> zones_;

    std::thread worker_thread_;
    std::thread io_thread_;
};
}

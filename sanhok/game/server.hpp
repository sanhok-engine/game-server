#pragma once

#include <boost/asio.hpp>
#include <sanhok/game/session.hpp>

#include <chrono>

namespace sanhok::game {
using namespace std::chrono;

class Server final {
public:
    Server();
    ~Server();

    void start();
    void stop();

private:
    std::atomic<bool> is_running_ {false};

    std::unique_ptr<Session> test_session_;
};
}

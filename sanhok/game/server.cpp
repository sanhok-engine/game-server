#include <sanhok/game/client.hpp>
#include <sanhok/game/server.hpp>

#include <chrono>

namespace sanhok::game {
Server::Server() {}

Server::~Server() {
    stop();
}

void Server::start() {
    if (is_running_.exchange(true)) return;

    test_session_ = std::make_unique<Session>(777, 50000, 1);
    test_session_->start();
}

void Server::stop() {
    if (!is_running_.exchange(false)) return;
    test_session_->stop();
}
}

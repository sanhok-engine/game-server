#include <sanhok/game/game_server.hpp>

int main() {
    using namespace sanhok::game;

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    GameServer server {};
    server.start();

    return 0;
}

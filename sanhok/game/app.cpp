#include <sanhok/game/server.hpp>

int main() {
    using namespace sanhok::game;

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    Server server {};
    server.start();

    return 0;
}

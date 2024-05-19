#include <sanhok/game/dummy/dummy_client.hpp>

int main()
{
    using namespace sanhok::game::dummy;

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif
    DummyClient client {};
    client.start(tcp::endpoint(tcp::v4(), 50000));

    return 0;
}

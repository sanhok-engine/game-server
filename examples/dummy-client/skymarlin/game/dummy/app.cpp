#include <../../../../../sanhok>

int main()
{
    using namespace skymarlin::game::dummy;

#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#endif

    boost::asio::io_context ctx {};

    const auto client = DummyClient::CreateDummyClient(ctx, "::1", 55555);
    client->start();

    auto work_guard = make_work_guard(ctx);
    ctx.run();

    return 0;
}

#pragma once

#include <../../../../../sanhok>
#include <../../../../../sanhok>

namespace skymarlin::game::dummy {
using boost::asio::ip::tcp;

class DummyClient final : public net::Client {
public:
    DummyClient(boost::asio::io_context& ctx, tcp::socket&& socket, net::ClientId id);

    static std::unique_ptr<DummyClient> CreateDummyClient(boost::asio::io_context& ctx, std::string_view host, unsigned int port);

private:
    void on_start() override;
    void on_stop() override;
    void handle_message(std::vector<uint8_t>&& buffer) override;

    void handle_player_update();

    std::unique_ptr<Player> player_;
};
}

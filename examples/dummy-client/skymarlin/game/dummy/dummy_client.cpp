#include <../../../../../sanhok>
#include <../../../../../sanhok>
#include <spdlog/spdlog.h>

namespace skymarlin::game::dummy {
DummyClient::DummyClient(boost::asio::io_context& ctx, tcp::socket&& socket, const net::ClientId id)
    : Client(ctx, std::move(socket), id) {}

std::unique_ptr<DummyClient> DummyClient::CreateDummyClient(boost::asio::io_context& ctx, std::string_view host,
    unsigned int port) {
    tcp::socket socket {ctx};
    tcp::resolver resolver {ctx};

    try {
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(host.data()), port));
    } catch (const boost::system::system_error& e) {
        spdlog::error("{}", e.what());
    }

    spdlog::info("Connected to {}:{}", host, port);
    return std::make_unique<DummyClient>(ctx, std::move(socket), 0);
}

void DummyClient::on_start() {
    spdlog::info("[DummyClient] starts");
}

void DummyClient::on_stop() {}

void DummyClient::handle_message(std::vector<uint8_t>&& buffer) {
    if (flatbuffers::Verifier verifier {buffer.data(), buffer.size()}; !msg::VerifyMessageBuffer(verifier)) {
        spdlog::error("[DummyClient] Verifying a message buffer has failed");
        stop();
        return;
    }

    switch (const auto message = msg::GetMessage(buffer.data()); message->message_type()) {
        using enum msg::MessageType;
    case PlayerEnter: {
        const auto enter = message->message_as<msg::PlayerEnter>();
        if (enter->type() != msg::PlayerEnterType::Greet) break;

        spdlog::info("[PlayerEnter] {}, network_id({})", enter->player_id(), enter->network_transform()->network_id());
        set_id(enter->player_id());
        player_ = std::make_unique<Player>(enter->network_transform()->network_id(), [this] {
            handle_player_update();
        });
        player_->Start();
    }

    default:
        break;
    }
}

void DummyClient::handle_player_update() {
    flatbuffers::FlatBufferBuilder builder {128};
    const msg::NetworkTransform network_transform {
        player_->network_id(), msg::Position(player_->position().x, player_->position().y, player_->position().z)
    };
    const auto update = CreateNetworkTransformUpdate(builder, &network_transform);
    builder.FinishSizePrefixed(CreateMessage(builder, msg::MessageType::NetworkTransformUpdate, update.Union()));

    send_message(std::make_shared<flatbuffers::DetachedBuffer>(builder.Release()));
}
}

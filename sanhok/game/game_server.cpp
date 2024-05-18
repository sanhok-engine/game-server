#include <sanhok/game/game_client.hpp>
#include <sanhok/game/game_server.hpp>

#include <chrono>

namespace sanhok::game {
GameServer::GameServer()
    : listener_(ctx_, LISTEN_PORT, [this](boost::asio::io_context& ctx, tcp::socket&& socket) {
        static std::atomic<ClientID> id_generator {0};

        const auto new_id = ++id_generator;
        clients_.insert_or_assign(new_id, std::make_shared<GameClient>(ctx, std::move(socket), new_id));
    }) {}

GameServer::~GameServer() {
    stop();
    if (worker_.joinable()) worker_.join();
}

void GameServer::start() {
    if (is_running_.exchange(true)) return;

    listener_.start();

    worker_ = std::thread([this] {
        auto last_tick_time = steady_clock::now();
        while (is_running_) {
            const auto current_time = steady_clock::now();
            const auto elapsed_time = duration_cast<milliseconds>(current_time - last_tick_time);
            if (elapsed_time < TICK_INTERVAL) continue;

            update(elapsed_time);
            last_tick_time = current_time;
        }
    });
    worker_.detach();

    ctx_.run();
}

void GameServer::stop() {
    if (!is_running_.exchange(false)) return;

    listener_.stop();
    clients_.clear();
}

void GameServer::update(const milliseconds dt) {
    flatbuffers::FlatBufferBuilder builder {1024 * 64};

    /*std::vector<msg::NetworkTransform> network_transforms;
    NetworkTransformManager::network_transforms.apply_all(
        [&network_transforms](std::shared_ptr<NetworkTransform> network_transform) {
            const auto& position = network_transform->transform().position;
            network_transforms.push_back({network_transform->network_id, {position.x, position.y, position.z}});
        });

    const auto state = CreateNetworkTransformState(builder, builder.CreateVectorOfStructs(network_transforms));
    builder.FinishSizePrefixed(CreateMessage(builder, msg::MessageType::NetworkTransformUpdate, state.Union()));

    net::ClientManager::broadcast(std::make_shared<flatbuffers::DetachedBuffer>(builder.Release()));*/
}
}

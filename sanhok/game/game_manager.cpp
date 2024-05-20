#include <sanhok/game/game_manager.hpp>

namespace sanhok::game {
GameManager::GameManager(const size_t zones)
    : zones_(zones) {}

void GameManager::stop() {
    for (auto& zone : zones_) {
        zone.clients.clear();
    }
}

void GameManager::update(const milliseconds dt) {
    for (auto& zone : zones_) {
        zone.update(dt);
    }
}

bool GameManager::player_join(std::shared_ptr<Client> client) {
    if (state_ != State::PREPARING) return false;

    // TODO: Spawn with random position
    client->player.movement.position = {0, 5, 0};

    return true;
}

void GameManager::player_leave(ClientID client_id) {
    for (auto& zone : zones_) {
        zone.clients.erase(client_id);
    }
}
}

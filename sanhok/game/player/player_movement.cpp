#include <glm/geometric.hpp>
#include <sanhok/game/player/player_movement.hpp>

#include <unordered_map>

namespace sanhok::game::player {
// m/s
static const std::unordered_map<PlayerMovementType, float> MOVEMENT_SPEEDS {
    {PlayerMovementType::IDLE, 0},
    {PlayerMovementType::STANDING_WALK, 1.7},
    {PlayerMovementType::STANDING_RUN, 4.7},
    {PlayerMovementType::STANDING_SPRINT, 6.3},
    {PlayerMovementType::CROUCH_WALK, 1.3},
    {PlayerMovementType::CROUCH_RUN, 3.4},
    {PlayerMovementType::CROUCH_SPRINT, 4.8},
    {PlayerMovementType::CRAWLING, 1.2},
    {PlayerMovementType::SWIMMING, 2.9},
};

void PlayerMovement::move(const std::chrono::milliseconds dt) {
    if (movement_type == PlayerMovementType::IDLE) return;
    position += normalize(body_direction) * (static_cast<float>(dt.count()) / 1000.0f * MOVEMENT_SPEEDS.at(movement_type));
}
}

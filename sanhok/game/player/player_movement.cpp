#include <glm/geometric.hpp>
#include <sanhok/game/player/player_movement.hpp>

#include <unordered_map>

namespace sanhok::game::player {
// m/s
static const std::unordered_map<PlayerMovement::Movement, float> MOVEMENT_SPEEDS {
    {PlayerMovement::Movement::IDLE, 0},
    {PlayerMovement::Movement::STANDING_WALK, 1.7},
    {PlayerMovement::Movement::STANDING_RUN, 4.7},
    {PlayerMovement::Movement::STANDING_SPRINT, 6.3},
    {PlayerMovement::Movement::CROUCH_WALK, 1.3},
    {PlayerMovement::Movement::CROUCH_RUN, 3.4},
    {PlayerMovement::Movement::CROUCH_SPRINT, 4.8},
    {PlayerMovement::Movement::CRAWLING, 1.2},
    {PlayerMovement::Movement::SWIMMING, 2.9},
};

void PlayerMovement::move(const std::chrono::milliseconds dt, const Movement movement) {
    if (movement == Movement::IDLE) return;
    position += normalize(look) * (static_cast<float>(dt.count()) / 1000 * MOVEMENT_SPEEDS.at(movement));
}
}

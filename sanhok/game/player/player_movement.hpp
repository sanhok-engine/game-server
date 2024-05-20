#pragma once

#include <glm/vec3.hpp>
#include <sanhok/game/protocol/player_movement.hpp>

#include <chrono>

namespace sanhok::game::player {
using protocol::PlayerMovementType;
using protocol::PlayerMovementDirection;

class PlayerMovement final {
public:
    void move(std::chrono::milliseconds dt);

    glm::vec3 position {};
    PlayerMovementType movement_type {PlayerMovementType::IDLE};
    PlayerMovementDirection movement_direction {PlayerMovementDirection::FORWARD};
    glm::vec3 body_direction {0, 0, 1.0f};
    glm::vec3 aim_direction {body_direction};
};
}

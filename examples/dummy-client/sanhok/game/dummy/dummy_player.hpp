#pragma once

#include <glm/vec3.hpp>
#include <sanhok/game/player/player_movement.hpp>

#include <chrono>
#include <random>

namespace sanhok::game::dummy {
using namespace std::chrono;
using protocol::PlayerMovementType;
using protocol::PlayerMovementDirection;

class DummyPlayer final {
public:
    DummyPlayer();

    void update(milliseconds dt);

    glm::vec3 position {};
    glm::vec3 body_direction {0, 0, 1};
    glm::vec3 aim_direction {body_direction};
    PlayerMovementType movement_type() const { return movement_type_; }
    PlayerMovementDirection movement_direction() const { return movement_direction_; }

private:
    void change_rotation(const time_point<steady_clock>& current_time);
    void change_movement(const time_point<steady_clock>& current_time);

    PlayerMovementType movement_type_ {PlayerMovementType::IDLE};
    PlayerMovementDirection movement_direction_ {PlayerMovementDirection::FORWARD};

    time_point<steady_clock> last_rotation_;
    time_point<steady_clock> last_movement_;
    std::random_device rd {};
    std::mt19937 gen {rd()};
};
}

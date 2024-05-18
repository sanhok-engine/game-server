#pragma once

#include <glm/vec3.hpp>

#include <chrono>

namespace sanhok::game::player {
class PlayerMovement final {
public:
    enum class Movement {
        IDLE,
        STANDING_WALK, STANDING_RUN, STANDING_SPRINT,
        CROUCH_WALK, CROUCH_RUN, CROUCH_SPRINT,
        CRAWLING,
        SWIMMING,
    };

    void move(std::chrono::milliseconds dt, Movement movement);

    glm::vec3 position {};
    glm::vec3 look {0, 0, 1.0};
};
}

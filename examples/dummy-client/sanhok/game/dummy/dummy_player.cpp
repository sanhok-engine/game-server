#include <glm/trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <sanhok/game/dummy/dummy_player.hpp>
#include <spdlog/spdlog.h>


namespace sanhok::game::dummy {
DummyPlayer::DummyPlayer() {
    last_rotation_ = steady_clock::now();
    last_movement_ = steady_clock::now();
}

void DummyPlayer::update(const milliseconds dt) {
    const auto current_time = steady_clock::now();
    change_rotation(current_time);
    change_movement(current_time);
}

void DummyPlayer::change_rotation(const time_point<steady_clock>& current_time) {
    static constexpr milliseconds rotation_interval {5000ms};
    static constexpr glm::vec3 up {0.0f, 1.0f, 0.0f};
    static std::uniform_real_distribution<float> distribution {glm::radians(-90.0f), glm::radians(90.0f)};

    if (current_time - last_rotation_ < rotation_interval) return;
    last_rotation_ = current_time;

    const auto rotation_radians = distribution(gen);
    const glm::quat rotation = glm::angleAxis(rotation_radians, up);
    body_direction = glm::normalize(rotation * body_direction);
    aim_direction = body_direction;

    spdlog::info("[DummyPlayer] Rotated to ({}, {}, {})", body_direction.x, body_direction.y, body_direction.z);
}

void DummyPlayer::change_movement(const time_point<steady_clock>& current_time) {
    static constexpr milliseconds movement_interval {3000ms};
    static constexpr std::array movements_types {
        PlayerMovementType::IDLE, PlayerMovementType::STANDING_WALK,
        PlayerMovementType::STANDING_RUN, PlayerMovementType::STANDING_SPRINT,
    };
    static constexpr std::array movements_directions {
        PlayerMovementDirection::FORWARD, PlayerMovementDirection::BACKWARD,
        PlayerMovementDirection::LEFT, PlayerMovementDirection::RIGHT,
    };
    static std::uniform_int_distribution<int> distribution_types {0, movements_types.size() - 1};
    static std::uniform_int_distribution<int> distribution_directions {0, movements_directions.size() - 1};

    if (current_time - last_movement_ < movement_interval) return;
    last_movement_ = current_time;

    movement_type_ = movements_types[distribution_types(gen)];
    spdlog::info("[DummyPlayer] Changed movement");
}
}

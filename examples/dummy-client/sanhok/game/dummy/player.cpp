#include <../../../../../sanhok>

#include <chrono>
#include <random>
#include <thread>

namespace skymarlin::game::dummy {
using namespace std::chrono_literals;

glm::vec3 GetRandomDirection();

Player::Player(const uint32_t network_id, std::function<void()> update_function)
    : network_id_(network_id), OnUpdate(std::move(update_function)) {}


void Player::Start() {
    running_ = true;

    std::thread ai_thread([this] {
        constexpr int UPDATES_PER_SECOND = 30;
        constexpr std::chrono::milliseconds UPDATE_INTERVAL {1000 / UPDATES_PER_SECOND};
        constexpr float SPEED = 10.0f;

        auto last_time = std::chrono::steady_clock::now();
        auto move_timeout = std::chrono::steady_clock::now();
        auto direction = GetRandomDirection();

        while (running_) {
            const auto current_time = std::chrono::steady_clock::now();
            const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_time);
            if (elapsed_time < UPDATE_INTERVAL) continue;

            if (current_time >= move_timeout) {
                move_timeout = current_time + 2s;
                direction = GetRandomDirection();
                continue;
            }

            const auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(elapsed_time).count();
            position_ += direction * (SPEED * dt);

            OnUpdate();

            last_time = current_time;
        }
    });
    ai_thread.detach();
}

void Player::Stop() {
    running_ = false;
}

glm::vec3 GetRandomDirection() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    return normalize(glm::vec3(dist(gen), 0.0f, dist(gen)));
}
}

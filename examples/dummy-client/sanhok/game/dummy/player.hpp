#pragma once

#include <glm/glm.hpp>

#include <atomic>
#include <functional>
#include <memory>

namespace skymarlin::game::dummy {
class Player {
public:
    Player(uint32_t network_id, std::function<void()> update_function);

    void Start();
    void Stop();

    uint32_t network_id() const { return network_id_; }
    const glm::vec3& position() const { return position_; }
    std::function<void()> OnUpdate;

private:
    std::atomic<bool> running_ {false};
    const uint32_t network_id_;
    glm::vec3 position_ {};
};
}

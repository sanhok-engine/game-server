#pragma once

#include <sanhok/game/client.hpp>
#include <sanhok/game/zone.hpp>

namespace sanhok::game {
class GameManager {
public:
    enum class State { PREPARING, PLAYING, FINISHED };

    explicit GameManager(size_t zones);

    void stop();
    void update(milliseconds dt);
    bool player_join(std::shared_ptr<Client> client);
    void player_leave(ClientID client_id);

    State state() const { return state_; }

private:
    std::atomic<State> state_ {State::PREPARING};
    std::vector<Zone> zones_;
};
}

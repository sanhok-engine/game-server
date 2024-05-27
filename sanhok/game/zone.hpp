#pragma once

#include <sanhok/container/concurrent_map.hpp>
#include <sanhok/game/client.hpp>
#include <sanhok/net/connection_udp.hpp>

namespace sanhok::game {
class Zone {
public:
    void update(milliseconds dt);

    ConcurrentMap<ClientID, std::shared_ptr<Client>> clients {};
};
}
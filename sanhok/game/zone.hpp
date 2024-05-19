#pragma once

#include <sanhok/concurrent_map.hpp>
#include <sanhok/game/client.hpp>

namespace sanhok::game {
class Zone {
public:
    void broadcast_tcp(std::unique_ptr<flatbuffers::DetachedBuffer> buffer);
    void broadcast_udp(std::unique_ptr<flatbuffers::DetachedBuffer> buffer);

    ConcurrentMap<ClientID, std::shared_ptr<Client>> clients {};
};
}
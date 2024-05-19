#include <sanhok/game/zone.hpp>

namespace sanhok::game {
void Zone::broadcast_tcp(std::unique_ptr<flatbuffers::DetachedBuffer> buffer, ClientID exception = 0) {
    clients.apply_all([exception](std::shared_ptr<Client>& client,
        const std::shared_ptr<flatbuffers::DetachedBuffer>& buffer) {
            if (client->id == exception) return;
            client->send_tcp(buffer);
        }, std::shared_ptr<flatbuffers::DetachedBuffer>(buffer.release()));
}

void Zone::broadcast_udp(std::unique_ptr<flatbuffers::DetachedBuffer> buffer, ClientID exception) {
    clients.apply_all([exception](std::shared_ptr<Client>& client,
        const std::shared_ptr<flatbuffers::DetachedBuffer>& buffer) {
            if (client->id == exception) return;
            client->send_udp(buffer);
        }, std::shared_ptr<flatbuffers::DetachedBuffer>(buffer.release()));
}
}

#include <sanhok/game/zone.hpp>

namespace sanhok::game {
void Zone::broadcast_tcp(std::unique_ptr<flatbuffers::DetachedBuffer> buffer) {
    clients.apply_all([](std::shared_ptr<Client>& client,
        const std::shared_ptr<flatbuffers::DetachedBuffer>& buffer) {
            client->send_tcp(buffer);
        }, std::shared_ptr<flatbuffers::DetachedBuffer>(buffer.release()));
}

void Zone::broadcast_udp(std::unique_ptr<flatbuffers::DetachedBuffer> buffer) {
    clients.apply_all([](std::shared_ptr<Client>& client,
        const std::shared_ptr<flatbuffers::DetachedBuffer>& buffer) {
            client->send_udp(buffer);
        }, std::shared_ptr<flatbuffers::DetachedBuffer>(buffer.release()));
}
}

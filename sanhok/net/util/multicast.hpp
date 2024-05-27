#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::udp;

static bool try_bind_free_multicast_group(udp::socket& socket, int max_try = 10) {
    static std::atomic<uint8_t> family4 = 0;

    int tries = 0;
    while (tries++ < max_try) {
        try {
            const auto address = boost::asio::ip::make_address_v4(std::format("239.255.0.{}", family4));


            return true;
        } catch (const std::exception& e) {
            continue;
        }
    }
    return false;
}
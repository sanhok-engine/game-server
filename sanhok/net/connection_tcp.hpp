#pragma once

#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <flatbuffers/flatbuffers.h>
#include <sanhok/container/concurrent_queue.hpp>
#include <spdlog/spdlog.h>

namespace sanhok::net {
using boost::asio::ip::tcp;

class ConnectionTCP final : boost::noncopyable {
public:
    ConnectionTCP(boost::asio::io_context& ctx, tcp::socket&& socket,
                  std::function<void()>&& on_connection, std::function<void()>&& on_disconnection,
                  std::function<void(std::vector<uint8_t>&&)>&& on_message);
    ~ConnectionTCP();

    void run();
    boost::asio::awaitable<bool> connect(const tcp::endpoint& remote_endpoint);
    void disconnect();
    void send_message(std::shared_ptr<flatbuffers::DetachedBuffer> message);
    void set_no_delay(bool no_delay);

    bool is_connected() const { return is_connected_; }
    tcp::endpoint local_endpoint() const { return socket_.local_endpoint(); }
    tcp::endpoint remote_endpoint() const { return socket_.remote_endpoint(); }

private:
    boost::asio::awaitable<void> receive_message();
    const std::function<void()> on_connection_;
    const std::function<void()> on_disconnection_;
    const std::function<void(std::vector<uint8_t>&&)> on_message_;

    boost::asio::io_context& ctx_;
    tcp::socket socket_;
    std::atomic<bool> is_connected_;

    ConcurrentQueue<std::vector<uint8_t>> receive_queue_;
    ConcurrentQueue<std::shared_ptr<flatbuffers::DetachedBuffer>> send_queue_ {};
    std::atomic<bool> is_sending_ {false};
    std::thread worker_thread_;
};


inline ConnectionTCP::ConnectionTCP(boost::asio::io_context& ctx, tcp::socket&& socket,
                                    std::function<void()>&& on_connection, std::function<void()>&& on_disconnection,
                                    std::function<void(std::vector<uint8_t>&&)>&& on_message)
    : on_connection_(std::move(on_connection)),
    on_disconnection_(std::move(on_disconnection)),
    on_message_(std::move(on_message)),
    ctx_(ctx), socket_(std::move(socket)), is_connected_(socket_.is_open()) {
    if (is_connected_) on_connection_();
}

inline ConnectionTCP::~ConnectionTCP() {
    disconnect();

    if (worker_thread_.joinable()) worker_thread_.join();
}

inline void ConnectionTCP::run() {
    // Start receiving messages
    co_spawn(ctx_, [this]()->boost::asio::awaitable<void> {
        while (is_connected_) {
            co_await receive_message();
        }
    }, boost::asio::detached);

    // Start handling messages
    worker_thread_ = std::thread([this] {
        while (is_connected_) {
            auto message = receive_queue_.pop_wait();
            if (!message) return;
            on_message_(std::move(*message));
        }
    });
    worker_thread_.detach();
}

inline boost::asio::awaitable<bool> ConnectionTCP::connect(const tcp::endpoint& remote_endpoint) {
    if (is_connected_) {
        spdlog::error("[ConnectionTCP] Socket is already connected");
        co_return false;
    }

    const auto [ec] = co_await socket_.async_connect(remote_endpoint, as_tuple(boost::asio::use_awaitable));
    if (ec) {
        spdlog::error("[ConnectionTCP] Error connecting to {}:{}", remote_endpoint.address().to_string(),
            remote_endpoint.port());
        co_return false;
    }

    is_connected_ = true;
    on_connection_();

    co_return true;
}

inline void ConnectionTCP::disconnect() {
    if (!is_connected_.exchange(false)) return;

    receive_queue_.clear();

    try {
        socket_.shutdown(tcp::socket::shutdown_both);

        //TODO: Process remaining send queue?
        socket_.close();
    } catch (const boost::system::system_error& e) {
        spdlog::error("[ConnectionTCP] Error shutting down socket: {}", e.what());
    }

    on_disconnection_();
}

inline void ConnectionTCP::send_message(std::shared_ptr<flatbuffers::DetachedBuffer> message) {
    if (!message || !is_connected_) return;

    send_queue_.push(std::move(message));

    // Send all messages in send_queue_
    if (is_sending_.exchange(true)) return;
    co_spawn(ctx_, [this]()->boost::asio::awaitable<void> {
        while (!send_queue_.empty()) {
            const auto message = send_queue_.pop();
            if (!message) continue;

            if (auto [ec, _] = co_await socket_.async_send(boost::asio::buffer((*message)->data(), (*message)->size()),
                as_tuple(boost::asio::use_awaitable)); ec) {
                spdlog::error("[ConnectionTCP] Error sending message: {}", ec.what());
                disconnect();
                co_return;
            }
        }

        is_sending_ = false;
    }, boost::asio::detached);
}

inline void ConnectionTCP::set_no_delay(const bool no_delay) {
    try {
        socket_.set_option(tcp::no_delay(no_delay));
    } catch (const boost::system::system_error& e) {
        spdlog::error("[ConnectionTCP] Error setting no-delay: {}", e.what());
    }
}

inline boost::asio::awaitable<void> ConnectionTCP::receive_message() {
    constexpr size_t MESSAGE_SIZE_PREFIX_BYTES = sizeof(flatbuffers::uoffset_t);

    std::array<uint8_t, MESSAGE_SIZE_PREFIX_BYTES> header_buffer {};
    if (const auto [ec, _] = co_await async_read(socket_,
        boost::asio::buffer(header_buffer),
        as_tuple(boost::asio::use_awaitable)); ec) {
        spdlog::error("[ConnectionTCP] Error on receiving header: {}", ec.what());
        disconnect();
        co_return;
    }

    const auto length = flatbuffers::GetSizePrefixedBufferLength(header_buffer.data());
    std::vector<uint8_t> body_buffer(length - MESSAGE_SIZE_PREFIX_BYTES);

    if (const auto [ec, _] = co_await async_read(socket_,
        boost::asio::buffer(body_buffer),
        as_tuple(boost::asio::use_awaitable)); ec) {
        spdlog::error("[ConnectionTCP] Error on receiving body: {}", ec.what());
        disconnect();
        co_return;
    }

    receive_queue_.push(std::move(body_buffer));
}
}

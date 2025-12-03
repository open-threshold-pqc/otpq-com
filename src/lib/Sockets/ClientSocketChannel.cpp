#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <format>
#include <thread>
#include <chrono>
#include <algorithm>

#include <otpqcom/Sockets/ClientSocketChannel.h>

namespace otpq::network::sockets {
    ClientSocketChannel::ClientSocketChannel(NodeNetworkConfig cfg)
        : SocketChannel(std::move(cfg)) {
        setupClient();
    }

    ClientSocketChannel::~ClientSocketChannel() {
        if (stream_) {
            [[maybe_unused]] auto _ = streamFlush();
            std::fclose(stream_);
            stream_ = nullptr;
        }
        if (connSocket_ >= 0) {
            ::close(connSocket_);
            connSocket_ = -1;
        }
    }


    std::expected<void, std::string>
    ClientSocketChannel::connect(const NodeNetworkConfig &cfg) noexcept {
        sockaddr_in dest{};
        dest.sin_family = AF_INET;

        if (::inet_pton(AF_INET, cfg.ip().data(), &dest.sin_addr) <= 0) {
            return std::unexpected(std::format(
                "[ClientSocketChannel] nodeConnect(): invalid IP '{}'", cfg.ip()
            ));
        }

        dest.sin_port = ::htons(cfg.port());

        // Retry until connection succeeds
        while (::connect(connSocket_,
                         reinterpret_cast<sockaddr *>(&dest),
                         sizeof(dest)) < 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }

        stream_ = ::fdopen(connSocket_, "wb+");
        if (!stream_) {
            return std::unexpected(
                "[ClientSocketChannel] nodeConnect(): fdopen() failed"
            );
        }

        if (::setvbuf(
                stream_,
                reinterpret_cast<char *>(buffer_.get()),
                static_cast<int>(NETWORK_IO_BUFFER_MODE),
                NETWORK_IO_BUFFER_SIZE) != 0) {
            return std::unexpected(
                "[ClientSocketChannel] nodeConnect(): setvbuf() failed"
            );
        }

        return {};
    }

    std::expected<void, std::string>
    ClientSocketChannel::setBufferMode(IOBufferMode mode) const noexcept {
        if (!stream_) {
            return std::unexpected(
                "[ClientSocketChannel] setBufferMode(): no IO stream (connection not established)"
            );
        }

        if (::setvbuf(
                stream_,
                reinterpret_cast<char *>(buffer_.get()),
                static_cast<int>(mode),
                NETWORK_IO_BUFFER_SIZE) != 0) {
            return std::unexpected(
                "[ClientSocketChannel] setBufferMode(): setvbuf() failed"
            );
        }

        std::ranges::fill_n(buffer_.get(), NETWORK_IO_BUFFER_SIZE, std::uint8_t{0});
        return {};
    }

    std::expected<std::size_t, std::string>
    ClientSocketChannel::sendData(const void *data, std::size_t len) noexcept {
        if (!stream_) {
            return std::unexpected(
                "[ClientSocketChannel] sendData(): no IO stream available"
            );
        }

        const auto *bytes = static_cast<const unsigned char *>(data);
        std::size_t sentTotal = 0;

        while (sentTotal < len) {
            const auto sent = std::fwrite(
                bytes + sentTotal, 1,
                len - sentTotal, stream_
            );

            netMetrics_.bytesSent += sent;

            if (sent == 0) {
                return std::unexpected(std::format(
                    "[ClientSocketChannel] sendData(): write stalled at byte {}/{}",
                    sentTotal, len
                ));
            }

            sentTotal += sent;
        }

        return sentTotal;
    }


    std::expected<std::size_t, std::string>
    ClientSocketChannel::recvData(void *data, std::size_t len) noexcept {
        if (!stream_) {
            return std::unexpected(
                "[ClientSocketChannel] recvData(): no IO stream available"
            );
        }

        if (auto r = streamFlush(); !r) {
            return std::unexpected(r.error());
        }

        auto *bytes = static_cast<unsigned char *>(data);
        std::size_t recvTotal = 0;

        while (recvTotal < len) {
            const auto received = std::fread(
                bytes + recvTotal, 1,
                len - recvTotal, stream_
            );

            netMetrics_.bytesReceived += received;

            if (received == 0) {
                return std::unexpected(std::format(
                    "[ClientSocketChannel] recvData(): read stalled at byte {}/{}",
                    recvTotal, len
                ));
            }

            recvTotal += received;
        }

        return recvTotal;
    }


    std::expected<void, std::string>
    ClientSocketChannel::streamFlush() const noexcept {
        if (stream_ && std::fflush(stream_) != 0) {
            return std::unexpected(
                "[ClientSocketChannel] streamFlush(): fflush() failed"
            );
        }
        return {};
    }

    void ClientSocketChannel::setupClient() {
        connSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (connSocket_ < 0) {
            throw std::runtime_error(std::format(
                "[ClientSocketChannel] socket(): failed ({}:{})",
                netcfg_.ip(), netcfg_.port()
            ));
        }

        sockaddr_in local{};
        local.sin_family = AF_INET;

        if (::inet_pton(AF_INET, netcfg_.ip().data(), &local.sin_addr) <= 0) {
            ::close(connSocket_);
            connSocket_ = -1;
            throw std::runtime_error(std::format(
                "[ClientSocketChannel] invalid local IP ({})",
                netcfg_.ip()
            ));
        }

        local.sin_port = ::htons(netcfg_.port());

        if (auto r = setOption(connSocket_, helpers::SocketOptions::ReuseAddr); !r) {
            ::close(connSocket_);
            connSocket_ = -1;
            throw std::runtime_error(std::format(
                "[ClientSocketChannel] setOption(REUSEADDR): failed: {}",
                r.error()
            ));
        }

        if (::bind(
                connSocket_,
                reinterpret_cast<sockaddr *>(&local),
                sizeof(local)) < 0) {
            ::close(connSocket_);
            connSocket_ = -1;
            throw std::runtime_error(std::format(
                "[ClientSocketChannel] bind(): failed ({}:{})",
                netcfg_.ip(), netcfg_.port()
            ));
        }
    }
}

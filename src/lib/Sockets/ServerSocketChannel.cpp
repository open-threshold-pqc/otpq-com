#include <format>
#include <expected>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <otpqcom/NetworkConfig.h>
#include <otpqcom/Sockets/ServerSocketChannel.h>

namespace otpq::network::sockets {
    ServerSocketChannel::ServerSocketChannel(NodeNetworkConfig cfg)
        : SocketChannel(std::move(cfg)) {
        setupServer();
    }

    ServerSocketChannel::~ServerSocketChannel() {
        if (stream_) {
            [[maybe_unused]] auto _ = streamFlush();
            std::fclose(stream_);
            stream_ = nullptr;
        }
        if (connSocket_ >= 0) {
            ::close(connSocket_);
            connSocket_ = -1;
        }
        if (listenSocket_ >= 0) {
            ::close(listenSocket_);
            listenSocket_ = -1;
        }
    }

    std::expected<void, std::string>
    ServerSocketChannel::listen() const noexcept {
        if (::listen(listenSocket_, 1) < 0) {
            return std::unexpected(std::format(
                "[ServerSocketChannel] listen() failed ({}:{})",
                netcfg_.ip(), netcfg_.port()
            ));
        }
        return {};
    }

    std::expected<void, std::string>
    ServerSocketChannel::accept() noexcept {
        sockaddr_in peer{};
        socklen_t peerSize = sizeof(peer);

        connSocket_ = ::accept(
            listenSocket_,
            reinterpret_cast<sockaddr *>(&peer),
            &peerSize
        );

        if (connSocket_ < 0) {
            return std::unexpected(std::format(
                "[ServerSocketChannel] accept() failed ({}:{})",
                netcfg_.ip(), netcfg_.port()
            ));
        }

        stream_ = ::fdopen(connSocket_, "wb+");
        if (!stream_) {
            ::close(connSocket_);
            connSocket_ = -1;
            return std::unexpected(
                "[ServerSocketChannel] fdopen() failed: unable to create FILE* stream"
            );
        }

        if (::setvbuf(
                stream_,
                reinterpret_cast<char *>(buffer_.get()),
                static_cast<int>(NETWORK_IO_BUFFER_MODE),
                NETWORK_IO_BUFFER_SIZE
            ) != 0) {
            return std::unexpected(
                "[ServerSocketChannel] setvbuf() failed when setting initial buffer mode"
            );
        }

        return {};
    }

    std::expected<void, std::string>
    ServerSocketChannel::listenAndAccept() noexcept {
        if (auto r = listen(); !r) return r;
        return accept();
    }

    std::expected<void, std::string>
    ServerSocketChannel::setBufferMode(IOBufferMode mode) const noexcept {
        if (!stream_) {
            return std::unexpected(
                "[ServerSocketChannel] setBufferMode(): no IO stream (connection not accepted)"
            );
        }

        if (::setvbuf(
                stream_,
                reinterpret_cast<char *>(buffer_.get()),
                static_cast<int>(mode),
                NETWORK_IO_BUFFER_SIZE
            ) != 0) {
            return std::unexpected(
                "[ServerSocketChannel] setBufferMode(): failed to apply buffer mode"
            );
        }

        std::ranges::fill(buffer_.get(), buffer_.get() + NETWORK_IO_BUFFER_SIZE, 0u);
        return {};
    }

    std::expected<std::size_t, std::string>
    ServerSocketChannel::sendData(const void *data, std::size_t len) noexcept {
        if (!stream_) {
            return std::unexpected(
                "[ServerSocketChannel] sendData(): no IO stream (connection not accepted)"
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
                    "[ServerSocketChannel] sendData(): write stalled at byte {}/{}",
                    sentTotal, len
                ));
            }

            sentTotal += sent;
        }

        return sentTotal;
    }

    std::expected<std::size_t, std::string>
    ServerSocketChannel::recvData(void *data, std::size_t len) noexcept {
        if (!stream_) {
            return std::unexpected(
                "[ServerSocketChannel] recvData(): no IO stream (connection not accepted)"
            );
        }

        if (auto r = streamFlush(); !r) {
            return std::unexpected(r.error());
        }

        auto *bytes = static_cast<unsigned char *>(data);

        std::size_t recvTotal = 0;
        while (recvTotal < len) {
            const auto received =
                    std::fread(bytes + recvTotal, 1, len - recvTotal, stream_);

            netMetrics_.bytesReceived += received;

            if (received == 0) {
                return std::unexpected(std::format(
                    "[ServerSocketChannel] recvData(): read stalled at byte {}/{}",
                    recvTotal, len
                ));
            }

            recvTotal += received;
        }

        return recvTotal;
    }

    std::expected<void, std::string>
    ServerSocketChannel::streamFlush() const noexcept {
        if (stream_ && std::fflush(stream_) != 0) {
            return std::unexpected(
                "[ServerSocketChannel] streamFlush(): fflush() failed"
            );
        }
        return {};
    }

    void ServerSocketChannel::setupServer() {
        sockaddr_in serv{};
        serv.sin_family = AF_INET;

        serv.sin_port = ::htons(netcfg_.port());

        listenSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket_ < 0) {
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] socket(): failed ({}:{})",
                netcfg_.ip(), netcfg_.port()
            ));
        }

        // Non-throwing expected-based API
        if (auto r = setOption(listenSocket_, helpers::SocketOptions::ReuseAddr); !r) {
            ::close(listenSocket_);
            listenSocket_ = -1;
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] setOption(ReuseAddr): failed: {}",
                r.error()
            ));
        }

        if (::bind(
                listenSocket_,
                reinterpret_cast<sockaddr *>(&serv),
                sizeof(serv)
            ) < 0) {
            ::close(listenSocket_);
            listenSocket_ = -1;
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] bind() failed ({}:{})",
                netcfg_.ip(), netcfg_.port()
            ));
        }
    }
}

#include <format>
#include <expected>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <otpqcom/NetworkConfig.h>
#include <otpqcom/NetIO/ServerSocketChannel.h>

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

    std::expected<void, std::string> ServerSocketChannel::awaitConnection() const {
        if (::listen(listenSocket_, 1) < 0) {
            return std::unexpected(std::format(
                "[ServerSocketChannel] listen() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
        }
        return {};
    }

    std::expected<void, std::string> ServerSocketChannel::acceptConnection() {
        sockaddr_in peerSocket{};
        socklen_t peerSocketSize = sizeof(peerSocket);

        connSocket_ = ::accept(listenSocket_,
                               reinterpret_cast<sockaddr *>(&peerSocket),
                               &peerSocketSize);
        if (connSocket_ < 0) {
            return std::unexpected(std::format(
                "[ServerSocketChannel] accept() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
        }

        stream_ = ::fdopen(connSocket_, "wb+");
        if (!stream_) {
            return std::unexpected("[ServerSocketChannel] acceptConnection(): failed to create IO stream");
        }

        if (::setvbuf(stream_,
                      reinterpret_cast<char *>(buffer_.get()),
                      static_cast<int>(NETWORK_IO_BUFFER_MODE),
                      NETWORK_IO_BUFFER_SIZE) != 0) {
            return std::unexpected("[ServerSocketChannel] acceptConnection(): failed to set stream buffer");
        }

        return {};
    }

    std::expected<void, std::string> ServerSocketChannel::awaitAndServe() {
        if (auto res = awaitConnection(); !res) return res;
        return acceptConnection();
    }

    std::expected<void, std::string> ServerSocketChannel::setBufferMode(NetworkBufferMode mode) const {
        if (!stream_) {
            return std::unexpected(
                "[ServerSocketChannel] setBufferMode(): no IO stream available (connection not established)");
        }

        if (::setvbuf(stream_,
                      reinterpret_cast<char *>(buffer_.get()),
                      static_cast<int>(mode),
                      NETWORK_IO_BUFFER_SIZE) != 0) {
            return std::unexpected("[ServerSocketChannel] setBufferMode(): failed to set buffer mode");
        }

        std::fill_n(buffer_.get(), NETWORK_IO_BUFFER_SIZE, 0);
        return {};
    }

    std::expected<std::size_t, std::string>
    ServerSocketChannel::sendData(const void *data, std::size_t len) {
        if (!stream_) {
            return std::unexpected("[ServerSocketChannel] sendData(): no IO stream available");
        }

        std::size_t dataSentBytes = 0;
        const auto *bytes = static_cast<const unsigned char *>(data);

        while (dataSentBytes < len) {
            const auto sent =
                std::fwrite(bytes + dataSentBytes, 1, len - dataSentBytes, stream_);

            netMetrics_.bytesSent += sent;

            if (sent == 0) {
                return std::unexpected(std::format(
                    "[ServerSocketChannel] sendData(): failed at byte {}/{}",
                    dataSentBytes, len));
            }
            dataSentBytes += sent;
        }
        return dataSentBytes;
    }

    std::expected<std::size_t, std::string>
    ServerSocketChannel::recvData(void *data, std::size_t len) {
        if (!stream_) {
            return std::unexpected("[ServerSocketChannel] recvData(): no IO stream available");
        }

        if (auto res = streamFlush(); !res) {
            return std::unexpected(res.error());
        }

        std::size_t dataReceivedBytes = 0;
        while (dataReceivedBytes < len) {
            const auto received =
                std::fread(static_cast<unsigned char *>(data) + dataReceivedBytes,
                           1, len - dataReceivedBytes, stream_);

            netMetrics_.bytesReceived += received;

            if (received > 0) {
                dataReceivedBytes += received;
            } else {
                return std::unexpected(std::format(
                    "[ServerSocketChannel] recvData(): failed at byte {}/{}",
                    dataReceivedBytes, len));
            }
        }
        return dataReceivedBytes;
    }

    std::expected<void, std::string> ServerSocketChannel::streamFlush() const {
        if (stream_ && std::fflush(stream_) != 0) {
            return std::unexpected("[ServerSocketChannel] streamFlush(): flush failed");
        }
        return {};
    }

    // unchanged: still throws on error
    void ServerSocketChannel::setupServer() {
        sockaddr_in serv{};
        serv.sin_family = AF_INET;
        if (::inet_pton(AF_INET, netcfg_.ip().data(), &serv.sin_addr) <= 0) {
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] invalid IP address ({})",
                netcfg_.ip()));
        }

        serv.sin_port = ::htons(netcfg_.base_port());

        listenSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket_ < 0) {
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] socket() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
        }

        setOption(listenSocket_, SocketOptions::REUSEADDR);

        if (::bind(listenSocket_,
                   reinterpret_cast<sockaddr *>(&serv),
                   sizeof(serv)) < 0) {
            ::close(listenSocket_);
            listenSocket_ = -1;
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] bind() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
        }
    }

} // namespace otpq::network::sockets

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <format>
#include <thread>
#include <chrono>

#include <otpqcom/NetIO/ClientSocketChannel.h>
#include <otpqcom/NetworkConfig.h>

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
    ClientSocketChannel::nodeConnect(const NodeNetworkConfig &cfg) {
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = ::inet_addr(cfg.ip().data());
        dest.sin_port = ::htons(cfg.base_port());

        // Retry until connection succeeds
        while (::connect(connSocket_, reinterpret_cast<sockaddr *>(&dest), sizeof(dest)) < 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }

        stream_ = ::fdopen(connSocket_, "wb+");
        if (!stream_) {
            return std::unexpected("[ClientSocketChannel] nodeConnect(): failed to create IO stream");
        }

        if (::setvbuf(stream_, reinterpret_cast<char *>(buffer_.get()),
                      static_cast<int>(NETWORK_IO_BUFFER_MODE),
                      NETWORK_IO_BUFFER_SIZE) != 0) {
            return std::unexpected("[ClientSocketChannel] nodeConnect(): failed to allocate stream buffer");
        }

        return {};
    }

    std::expected<void, std::string>
    ClientSocketChannel::setBufferMode(NetworkBufferMode mode) const {
        if (!stream_) {
            return std::unexpected(
                "[ClientSocketChannel] setBufferMode(): no IO stream available (connection not established)");
        }

        if (::setvbuf(stream_,
                      reinterpret_cast<char *>(buffer_.get()),
                      static_cast<int>(mode),
                      NETWORK_IO_BUFFER_SIZE) != 0) {
            return std::unexpected("[ClientSocketChannel] setBufferMode(): failed to set buffer mode");
        }

        std::fill_n(buffer_.get(), NETWORK_IO_BUFFER_SIZE, 0);
        return {};
    }

    std::expected<std::size_t, std::string>
    ClientSocketChannel::sendData(const void *data, std::size_t len) {
        if (!stream_) {
            return std::unexpected("[ClientSocketChannel] sendData(): no IO stream available");
        }

        std::size_t dataSentBytes = 0;
        const auto *bytes = static_cast<const unsigned char *>(data);

        while (dataSentBytes < len) {
            const auto sent =
                    std::fwrite(bytes + dataSentBytes, 1, len - dataSentBytes, stream_);

            netMetrics_.bytesSent += sent;

            if (sent == 0) {
                return std::unexpected(std::format(
                    "[ClientSocketChannel] sendData(): failed at byte {}/{}", dataSentBytes, len));
            }
            dataSentBytes += sent;
        }

        return dataSentBytes;
    }

    std::expected<std::size_t, std::string>
    ClientSocketChannel::recvData(void *data, std::size_t len) {
        if (!stream_) {
            return std::unexpected("[ClientSocketChannel] recvData(): no IO stream available");
        }

        if (auto res = streamFlush(); !res) return std::unexpected(res.error());

        std::size_t dataReceivedBytes = 0;
        auto *bytes = static_cast<unsigned char *>(data);

        while (dataReceivedBytes < len) {
            const auto received =
                    std::fread(bytes + dataReceivedBytes, 1, len - dataReceivedBytes, stream_);

            netMetrics_.bytesReceived += received;

            if (received > 0) {
                dataReceivedBytes += received;
            } else {
                return std::unexpected(std::format(
                    "[ClientSocketChannel] recvData(): failed at byte {}/{}", dataReceivedBytes, len));
            }
        }

        return dataReceivedBytes;
    }

    std::expected<void, std::string>
    ClientSocketChannel::streamFlush() const {
        if (stream_ && std::fflush(stream_) != 0) {
            return std::unexpected("[ClientSocketChannel] streamFlush(): flush failed");
        }
        return {};
    }

    // still exception-based for constructor safety
    void ClientSocketChannel::setupClient() {
        connSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (connSocket_ < 0) {
            throw std::runtime_error(std::format(
                "[ClientSocketChannel] socket() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
        }

        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = ::inet_addr(netcfg_.ip().data());
        local.sin_port = ::htons(netcfg_.base_port());

        setOption(connSocket_, SocketOptions::REUSEADDR);

        if (::bind(connSocket_, reinterpret_cast<sockaddr *>(&local), sizeof(local)) < 0) {
            ::close(connSocket_);
            connSocket_ = -1;
            throw std::runtime_error(std::format(
                "[ClientSocketChannel] bind() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
        }
    }
} // namespace otpq::network::sockets

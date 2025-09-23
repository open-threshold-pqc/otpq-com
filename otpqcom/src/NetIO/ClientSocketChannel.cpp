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
            streamFlush();
            std::fclose(stream_);
            stream_ = nullptr;
        }

        if (connSocket_ >= 0) {
            ::close(connSocket_);
            connSocket_ = -1;
        }
    }

    void ClientSocketChannel::nodeConnect(const NodeNetworkConfig &cfg) {
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = ::inet_addr(cfg.ip().data());
        dest.sin_port = ::htons(cfg.base_port());

        /* Retry until connection succeeds */
        while (::connect(connSocket_, reinterpret_cast<sockaddr *>(&dest), sizeof(dest)) < 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }

        stream_ = ::fdopen(connSocket_, "wb+");
        if (!stream_)
            throw std::runtime_error("[ClientSocketChannel] nodeConnect(): failed to create IO stream");


        if (::setvbuf(stream_, reinterpret_cast<char *>(buffer_.get()),
                      static_cast<int>(NETWORK_IO_BUFFER_MODE),
                      NETWORK_IO_BUFFER_SIZE) != 0) {
            throw std::runtime_error("[ClientSocketChannel] nodeConnect(): failed to allocate stream buffer");
        }
    }

    void ClientSocketChannel::setBufferMode(NetworkBufferMode mode) const {
        if (!stream_)
            throw std::runtime_error(
                "[ServerSocketChannel] setBufferMode(): no IO stream available (connection not established)"
            );

        if (::setvbuf(stream_,
                      reinterpret_cast<char *>(buffer_.get()),
                      static_cast<int>(mode),
                      NETWORK_IO_BUFFER_SIZE) != 0)
            throw std::runtime_error("[ServerSocketChannel] setBufferMode(): failed to set buffer mode");

        /* Zero out the buffer */
        std::fill_n(buffer_.get(), NETWORK_IO_BUFFER_SIZE, 0);
    }

    void ClientSocketChannel::sendData(const void *data, std::size_t len) {
        if (!stream_)
            throw std::runtime_error("[ClientSocketChannel] sendData(): no IO stream available");

        std::size_t dataSentBytes = 0;
        const auto *bytes = static_cast<const unsigned char *>(data);

        while (dataSentBytes < len) {
            const auto sent = std::fwrite(bytes + dataSentBytes, 1, len - dataSentBytes, stream_);
            if (sent == 0)
                throw std::runtime_error(std::format(
                    "[ClientSocketChannel] sendData(): failed at byte {}/{}", dataSentBytes, len));

            dataSentBytes += sent;
        }
    }

    void ClientSocketChannel::recvData(void *data, std::size_t len) {
        if (!stream_) {
            throw std::runtime_error("[ClientSocketChannel] recvData(): no IO stream available");
        }

        streamFlush();

        std::size_t dataReceivedBytes = 0;
        auto *bytes = static_cast<unsigned char *>(data);

        while (dataReceivedBytes < len) {
            const auto received = std::fread(bytes + dataReceivedBytes, 1, len - dataReceivedBytes, stream_);
            if (received > 0)
                dataReceivedBytes += received;
            else
                throw std::runtime_error(std::format(
                    "[ClientSocketChannel] recvData(): failed at byte {}/{}", dataReceivedBytes, len));
        }
    }

    void ClientSocketChannel::streamFlush() const {
        if (stream_ && std::fflush(stream_) != 0)
            throw std::runtime_error("[ClientSocketChannel] streamFlush(): flush failed");
    }

    void ClientSocketChannel::setupClient() {
        connSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (connSocket_ < 0)
            throw std::runtime_error(std::format("[ClientSocketChannel] socket() failed ({}, {})",
                                                 netcfg_.ip(), netcfg_.base_port()));

        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = ::inet_addr(netcfg_.ip().data());
        local.sin_port = ::htons(netcfg_.base_port());

        setOption(connSocket_, SocketOptions::REUSEADDR);

        if (::bind(connSocket_, reinterpret_cast<sockaddr *>(&local), sizeof(local)) < 0) {
            ::close(connSocket_);
            connSocket_ = -1;
            throw std::runtime_error(std::format("[ClientSocketChannel] bind() failed ({}, {})",
                                                 netcfg_.ip(), netcfg_.base_port()));
        }
    }
} // namespace otpq::network::sockets

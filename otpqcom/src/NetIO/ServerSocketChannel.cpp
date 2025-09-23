#include <format>
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
            streamFlush();
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

    void ServerSocketChannel::awaitConnection() const {
        if (::listen(listenSocket_, 1) < 0)
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] listen() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
    }

    void ServerSocketChannel::acceptConnection() {
        // TODO: consider closing any existing connSocket_ if still open

        sockaddr_in peerSocket{};
        socklen_t peerSocketSize = sizeof(peerSocket);

        connSocket_ = ::accept(listenSocket_,
                               reinterpret_cast<sockaddr *>(&peerSocket),
                               &peerSocketSize);
        if (connSocket_ < 0) {
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] accept() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));
        }

        stream_ = ::fdopen(connSocket_, "wb+");
        if (!stream_)
            throw std::runtime_error("[ServerSocketChannel] acceptConnection(): failed to create IO stream");


        if (::setvbuf(stream_,
                      reinterpret_cast<char *>(buffer_.get()),
                      static_cast<int>(NETWORK_IO_BUFFER_MODE),
                      NETWORK_IO_BUFFER_SIZE) != 0)
            throw std::runtime_error("[ServerSocketChannel] acceptConnection(): failed to set stream buffer");
    }

    void ServerSocketChannel::awaitAndServe() {
        awaitConnection();
        acceptConnection();
    }

    void ServerSocketChannel::setBufferMode(NetworkBufferMode mode) const {
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

    void ServerSocketChannel::sendData(const void *data, std::size_t len) {
        if (!stream_)
            throw std::runtime_error("[ServerSocketChannel] sendData(): no IO stream available");


        std::size_t dataSentBytes = 0;
        const auto *bytes = static_cast<const unsigned char *>(data);

        while (dataSentBytes < len) {
            const auto sent = std::fwrite(bytes + dataSentBytes, 1, len - dataSentBytes, stream_);
            if (sent == 0) {
                throw std::runtime_error(std::format(
                    "[ServerSocketChannel] sendData(): failed at byte {}/{}",
                    dataSentBytes, len));
            }
            dataSentBytes += sent;
        }
    }

    void ServerSocketChannel::recvData(void *data, std::size_t len) {
        if (!stream_)
            throw std::runtime_error("[ServerSocketChannel] recvData(): no IO stream available");

        streamFlush();

        std::size_t dataReceivedBytes = 0;

        while (dataReceivedBytes < len) {
            const auto received = std::fread(static_cast<unsigned char *>(data) + dataReceivedBytes, 1,
                                             len - dataReceivedBytes, stream_);
            if (received > 0) {
                dataReceivedBytes += received;
            } else {
                throw std::runtime_error(std::format(
                    "[ServerSocketChannel] recvData(): failed at byte {}/{}",
                    dataReceivedBytes, len));
            }
        }
    }

    void ServerSocketChannel::streamFlush() const {
        if (stream_ && std::fflush(stream_) != 0)
            throw std::runtime_error("[ServerSocketChannel] streamFlush(): flush failed");
    }

    void ServerSocketChannel::setupServer() {
        sockaddr_in serv{};
        serv.sin_family = AF_INET;
        if (::inet_pton(AF_INET, netcfg_.ip().data(), &serv.sin_addr) <= 0)
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] invalid IP address ({})",
                netcfg_.ip()));

        serv.sin_port = ::htons(netcfg_.base_port());

        listenSocket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket_ < 0)
            throw std::runtime_error(std::format(
                "[ServerSocketChannel] socket() failed ({}, {})",
                netcfg_.ip(), netcfg_.base_port()));


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

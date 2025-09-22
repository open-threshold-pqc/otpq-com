#include <format>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
            fclose(stream_);
        }

        if (listenSocket_ >= 0)
            close(listenSocket_);

        if (connSocket_ >= 0)
            close(connSocket_);
    }

    void ServerSocketChannel::awaitConnection() const {
        if (listen(listenSocket_, 1) < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server listen() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));
    }

    void ServerSocketChannel::acceptConnection() {
        //todo check for previous open connection

        sockaddr_in peerSocket{};
        socklen_t peerSocketSize = sizeof(peerSocket);
        connSocket_ = accept(listenSocket_, reinterpret_cast<sockaddr *>(&peerSocket), &peerSocketSize);
        if (connSocket_ < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server accept() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));

        stream_ = fdopen(connSocket_, "wb+");
        if (!stream_)
            throw std::runtime_error("[ServerSocketChannel]: Server acceptConnection() failed creating an IO channel");

        if (setvbuf(stream_, reinterpret_cast<char *>(buffer_.get()),
                    static_cast<int>(NetworkBufferMode::FullyBuffered),
                    NETWORK_IO_BUFFER_SIZE) != 0)
            throw std::runtime_error(
                "[ServerSocketChannel]: acceptConnection() failed allocating buffer for IO stream");
    }

    void ServerSocketChannel::awaitAndServe() {
        awaitConnection();
        acceptConnection();
    }

    void ServerSocketChannel::sendData(const void *data, std::size_t len) {
        if (!stream_)
            throw std::runtime_error("[ServerSocketChannel] sendData() failed. No IO stream exists.");

        std::size_t dataSentBytes = 0;

        while (dataSentBytes < len) {
            const auto sendResult = fwrite(static_cast<const char *>(data) + dataSentBytes, 1, len - dataSentBytes,
                                           stream_);
            if (sendResult == 0)
                throw std::runtime_error(std::format("[ServerSocketChannel] sendData() failed at byte {}/{}",
                                                     dataSentBytes, len));

            dataSentBytes += sendResult;
        }
    }

    void ServerSocketChannel::recvData(void *data, std::size_t len) {
        if (!stream_)
            throw std::runtime_error("[ServerSocketChannel] recvData() failed. No IO stream exists.");

        fflush(stream_);

        // if (hasSent_) {
        // }
        // hasSent_ = false;

        std::size_t dataReceivedBytes{};

        while (dataReceivedBytes < len) {
            if (const auto res = fread(static_cast<char *>(data) + dataReceivedBytes, 1, len - dataReceivedBytes,
                                       stream_); res > 0)
                dataReceivedBytes += res;
            else
                throw std::runtime_error(std::format("[ServerSocketChannel] recvData() failed at byte {}/{}",
                                                     dataReceivedBytes, len));
        }
    }

    void ServerSocketChannel::streamFlush() const {
        if (stream_)
            fflush(stream_);
    }

    void ServerSocketChannel::setupServer() {
        sockaddr_in serv{};
        serv.sin_family = AF_INET;
        if (inet_pton(AF_INET, netcfg_.ip().data(), &serv.sin_addr) <= 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server Ip set failed ({}, {}) ", netcfg_.ip(),
                                                 netcfg_.base_port()));

        serv.sin_port = htons(netcfg_.base_port());

        listenSocket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket_ < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server socket() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));

        setOption(listenSocket_, SocketOptions::REUSEADDR);

        if (bind(listenSocket_, reinterpret_cast<sockaddr *>(&serv), sizeof(serv)) < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server bind() failed ({}, {}) ", netcfg_.ip(),
                                                 netcfg_.base_port()));
    }
} // namespace otpq::network

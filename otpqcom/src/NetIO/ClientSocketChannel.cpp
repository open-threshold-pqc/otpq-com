#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <format>
#include <thread>
#include <chrono>
#include <iostream>

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
            fclose(stream_);
        }

        if (connSocket_ >= 0)
            close(connSocket_);
    }

    void ClientSocketChannel::nodeConnect(const NodeNetworkConfig &cfg) {
        sockaddr_in dest = {};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(cfg.ip().data());
        dest.sin_port = htons(cfg.base_port());

        while (true) {
            if (connect(connSocket_, reinterpret_cast<sockaddr *>(&dest), sizeof(sockaddr)) == 0)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }

        stream_ = fdopen(connSocket_, "wb+");
        if (!stream_)
            throw std::runtime_error("[ClientSocketChannel]: nodeConnect() failed creating an IO channel");

        if (setvbuf(stream_, reinterpret_cast<char *>(buffer_.get()),
                    static_cast<int>(NetworkBufferMode::FullyBuffered),
                    NETWORK_IO_BUFFER_SIZE) != 0)
            throw std::runtime_error("[ClientSocketChannel]: nodeConnect() failed allocating buffer for IO stream");
    }

    void ClientSocketChannel::sendData(const void *data, std::size_t len) {
        if (!stream_)
            throw std::runtime_error("[ClientSocketChannel] sendData() failed. No IO stream exists.");

        std::size_t dataSentBytes = 0;

        while (dataSentBytes < len) {
            const auto sendResult = fwrite(static_cast<const char *>(data) + dataSentBytes, 1, len - dataSentBytes,
                                           stream_);
            if (sendResult == 0)
                throw std::runtime_error(std::format("[ClientSocketChannel] sendData() failed at byte {}/{}",
                                                     dataSentBytes, len));

            dataSentBytes += sendResult;
        }
    }


    void ClientSocketChannel::recvData(void *data, std::size_t len) {
        if (!stream_)
            throw std::runtime_error("[ClientSocketChannel] recvData() failed. No IO stream exists.");

        fflush(stream_);


        std::size_t dataReceivedBytes{};

        while (dataReceivedBytes < len) {
            if (const auto res = fread(static_cast<char *>(data) + dataReceivedBytes, 1, len - dataReceivedBytes,
                                       stream_); res > 0)
                dataReceivedBytes += res;
            else
                throw std::runtime_error(std::format("[ClientSocketChannel] recvData() failed at byte {}/{}",
                                                     dataReceivedBytes, len));
        }
    }

    void ClientSocketChannel::streamFlush() const {
        if (stream_)
            fflush(stream_);
    }

    void ClientSocketChannel::setupClient() {
        connSocket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (connSocket_ < 0)
            throw std::runtime_error(std::format("[ClientSocketChannel] socket() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));

        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = inet_addr(netcfg_.ip().data());
        local.sin_port = htons(netcfg_.base_port());

        setOption(connSocket_, SocketOptions::REUSEADDR);

        if (bind(connSocket_, reinterpret_cast<sockaddr *>(&local), sizeof(local)) < 0) {
            close(connSocket_);
            throw std::runtime_error(std::format("[ClientSocketChannel] bind() failed ({}, {}) ", netcfg_.ip(),
                                                 netcfg_.base_port()));
        }
    }
} // namespace otpq::network

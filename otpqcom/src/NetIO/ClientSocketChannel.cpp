#include "otpqcom/NetIO/ClientSocketChannel.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <format>
#include <thread>
#include <chrono>

#include <iostream>

namespace otpq::network::sockets {
    ClientSocketChannel::ClientSocketChannel(NodeNetworkConfig cfg)
        : SocketChannel(std::move(cfg)) {
        setupClient();
    }

    ClientSocketChannel::~ClientSocketChannel() {
        // flush();
        // if (stream_) {
        //     fclose(stream_);
        // }
        // if (connSocket_ >= 0) {
        //     close(connSocket_);
        // }
    }

    void ClientSocketChannel::nodeConnect(const NodeNetworkConfig &cfg) {

        struct sockaddr_in dest = {};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(cfg.ip().data());
        dest.sin_port = htons(cfg.base_port());

        while(true) {
            connSocket_ = socket(AF_INET, SOCK_STREAM, 0);

            if (connect(connSocket_, reinterpret_cast<sockaddr *>(&dest), sizeof(sockaddr)) == 0)
                break;

            close(connSocket_);
            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }

        std::cout << "Connected\n";
        // setupClient(address, port);
        //
        // stream_ = fdopen(connSocket_, "wb+");
        // if (!stream_) {
        //     throw std::runtime_error("[SocketClient]: fdopen() failed");
        // }
        //
        // buffer_.assign(otpq::network::NETWORK_BUFFER_SIZE, 0);
        // setvbuf(stream_, buffer_.data(),
        //         static_cast<int>(otpq::network::NetworkBufferMode::FullyBuffered),
        //         otpq::network::NETWORK_BUFFER_SIZE);
    }

    void ClientSocketChannel::sendData(const void *data, std::size_t len) {
        // std::size_t sent = 0;
        // const auto *buf = static_cast<const char *>(data);
        //
        // while (sent < len) {
        //     const auto res = fwrite(buf + sent, 1, len - sent, stream_);
        //     if (res > 0) {
        //         sent += res;
        //     } else {
        //         throw std::runtime_error("[SocketClient]: send failed");
        //     }
        // }
        // hasSent_ = true;
    }

    void ClientSocketChannel::recvData(void *data, std::size_t len) {
        // if (hasSent_) {
        //     fflush(stream_);
        // }
        // hasSent_ = false;
        //
        // std::size_t received = 0;
        // auto *buf = static_cast<char *>(data);
        //
        // while (received < len) {
        //     const auto res = fread(buf + received, 1, len - received, stream_);
        //     if (res > 0) {
        //         received += res;
        //     } else {
        //         throw std::runtime_error("[SocketClient]: recv failed");
        //     }
        // }
    }

    // void SocketClient::setNoDelay() {
    //     constexpr int one{1};
    //     setsockopt(connSocket_, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
    // }
    //
    // void SocketClient::setDelay() {
    //     const int zero = 0;
    //     setsockopt(connSocket_, IPPROTO_TCP, TCP_NODELAY, &zero, sizeof(zero));
    // }
    //
    // void SocketClient::flush() {
    //     if (stream_) {
    //         fflush(stream_);
    //     }
    // }

    void ClientSocketChannel::setupClient() {
        connSocket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (connSocket_ < 0)
            throw std::runtime_error(std::format("[ClientSocketChannel]: Client socket() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));

        sockaddr_in local{};
        local.sin_family = AF_INET;
        local.sin_addr.s_addr = inet_addr(netcfg_.ip().data());
        local.sin_port = 0 ;// htons(netcfg_.base_port());

        if (bind(connSocket_, reinterpret_cast<sockaddr *>(&local), sizeof(local)) < 0) {
            close(connSocket_);
            throw std::runtime_error(std::format("[ClientSocketChannel]: Client bind() failed ({}, {}) ", netcfg_.ip(),
                                                 netcfg_.base_port()));
        }
    }

    void ClientSocketChannel::setOption(const SocketOptions opt) const {
        if (opt == SocketOptions::REUSEADDR) {
            constexpr int reuse{1};
            setsockopt(connSocket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        } else
            throw std::runtime_error(
                "[ClientSocketChannel]: setOption(), option is invalid/unsupported for this socket");
    }

} // namespace otpq::network

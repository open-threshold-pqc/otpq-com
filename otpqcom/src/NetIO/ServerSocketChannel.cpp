#include "otpqcom/NetIO/ServerSocketChannel.h"
#include <format>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace otpq::network::sockets {
    ServerSocketChannel::ServerSocketChannel(NodeNetworkConfig cfg)
        : SocketChannel(std::move(cfg)) {
        setupServer();
    }

    ServerSocketChannel::~ServerSocketChannel() {
        // flush();
        // if (stream_) {
        //     fclose(stream_);
        // }
        if (connSocket_ >= 0) {
           close(connSocket_);
        }
    }

    void ServerSocketChannel::beginListen() {
        if (listen(connSocket_, 1) < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server listen() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));

        sockaddr_in peerSocket{};
        socklen_t peerSocketSize = sizeof(peerSocket);
        const int peerConnSocket = accept(connSocket_, reinterpret_cast<sockaddr *>(&peerSocket), &peerSocketSize);
        if (peerConnSocket < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server accept() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));
        close(connSocket_);
        connSocket_ = peerConnSocket;
    }

    void ServerSocketChannel::sendData(const void *data, std::size_t len) {
        // std::size_t sent = 0;
        // const auto *buf = static_cast<const char *>(data);
        //
        // while (sent < len) {
        //     const auto res = fwrite(buf + sent, 1, len - sent, stream_);
        //     if (res > 0) {
        //         sent += res;
        //     } else {
        //         throw std::runtime_error("[SocketServer]: send failed");
        //     }
        // }
        // hasSent_ = true;
    }

    void ServerSocketChannel::recvData(void *data, std::size_t len) {
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
        //         throw std::runtime_error("[SocketServer]: recv failed");
        //     }
        // }
    }

    // void SocketServer::setNoDelay() {
    //     constexpr int one{1};
    //     setsockopt(connSocket_, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
    // }
    //
    // void SocketServer::setDelay() {
    //     const int zero = 0;
    //     setsockopt(connSocket_, IPPROTO_TCP, TCP_NODELAY, &zero, sizeof(zero));
    // }
    //
    // void SocketServer::flush() {
    //     if (stream_) {
    //         fflush(stream_);
    //     }
    // }

    void ServerSocketChannel::setupServer() {
        sockaddr_in serv{};
        serv.sin_family = AF_INET;
        if (inet_pton(AF_INET, netcfg_.ip().data(), &serv.sin_addr) <= 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server Ip set failed ({}, {}) ", netcfg_.ip(),
                                                 netcfg_.base_port()));

        serv.sin_port = htons(netcfg_.base_port());

        connSocket_ = socket(AF_INET, SOCK_STREAM, 0);
        if (connSocket_ < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server socket() failed ({}, {}) ",
                                                 netcfg_.ip(),
                                                 netcfg_.base_port()));

        setOption(SocketOptions::REUSEADDR);

        if (bind(connSocket_, reinterpret_cast<sockaddr *>(&serv), sizeof(serv)) < 0)
            throw std::runtime_error(std::format("[ServerSocketChannel]: Server bind() failed ({}, {}) ", netcfg_.ip(),
                                                 netcfg_.base_port()));
    }

    void ServerSocketChannel::setOption(const SocketOptions opt) const {
        if (opt == SocketOptions::REUSEADDR) {
            constexpr int reuse{1};
            setsockopt(connSocket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        } else
            throw std::runtime_error(
                "[ServerSocketChannel]: setOption(), option is invalid/unsupported for this socket");
    }
} // namespace otpq::network

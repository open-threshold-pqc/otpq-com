#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>

namespace otpq::network::sockets {
    SocketChannel::SocketChannel(NodeNetworkConfig cfg)
        : netcfg_(std::move(cfg)),
          buffer_(std::make_unique<uint8_t[]>(NETWORK_IO_BUFFER_SIZE)) {
        std::fill_n(buffer_.get(), NETWORK_IO_BUFFER_SIZE, 0);
    }

    SocketChannel::~SocketChannel() = default;

    void SocketChannel::setOption(const int connSocket, const SocketOptions opt) {
        if (connSocket < 0)
            throw std::runtime_error("[SocketChannel] setOption(): invalid socket descriptor (< 0)");


        constexpr int enable{1};
        constexpr int disable{0};
        switch (opt) {
            case SocketOptions::REUSEADDR:
                if (::setsockopt(connSocket, SOL_SOCKET, SO_REUSEADDR,
                                 &enable, sizeof(enable)) < 0) {
                    throw std::runtime_error("[SocketChannel] setOption(): failed to set SO_REUSEADDR");
                }
                break;

            case SocketOptions::SETDELAY:
                if (::setsockopt(connSocket, IPPROTO_TCP, TCP_NODELAY,
                                 &disable, sizeof(disable)) < 0) {
                    throw std::runtime_error("[SocketChannel] setOption(): failed to set TCP_NODELAY=0");
                }
                break;

            case SocketOptions::SETNODELAY:
                if (::setsockopt(connSocket, IPPROTO_TCP, TCP_NODELAY,
                                 &enable, sizeof(enable)) < 0) {
                    throw std::runtime_error("[SocketChannel] setOption(): failed to set TCP_NODELAY=1");
                }
                break;

            default:
                throw std::invalid_argument("[SocketChannel] setOption(): unsupported socket option");
        }
    }
} // namespace otpq::network::sockets

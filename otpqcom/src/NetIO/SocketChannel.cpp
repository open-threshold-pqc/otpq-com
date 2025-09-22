#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cstring>
#include <unistd.h>

#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>

namespace otpq::network::sockets {
    SocketChannel::SocketChannel(NodeNetworkConfig cfg)
        : netcfg_(std::move(cfg)),
          buffer_(std::make_unique<uint8_t[]>(NETWORK_IO_BUFFER_SIZE)) {
        std::memset(buffer_.get(), 0, NETWORK_IO_BUFFER_SIZE);
    }

    SocketChannel::~SocketChannel() {

    }

    void SocketChannel::setOption(const int connSocket, const SocketOptions opt) {
        if (connSocket < 0)
            throw std::runtime_error("[SocketChannel]: setOption(), socket is not valid ( < 0)");

        if (opt == SocketOptions::REUSEADDR) {
            constexpr int reuse{1};
            setsockopt(connSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        } else
            throw std::runtime_error(
                "[SocketChannel]: setOption(), option is invalid/unsupported for this socket");
    }
} // namespace otpq::network::sockets

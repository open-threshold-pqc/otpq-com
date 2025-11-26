#include <stdexcept>
#include <format>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>

namespace otpq::network::sockets {
    SocketChannel::SocketChannel(NodeNetworkConfig cfg)
        : netcfg_{std::move(cfg)},
          buffer_{std::make_unique<std::uint8_t[]>(NETWORK_IO_BUFFER_SIZE)} {
        std::ranges::fill_n(buffer_.get(), NETWORK_IO_BUFFER_SIZE, std::uint8_t{0});
    }

    SocketChannel::~SocketChannel() = default;

    std::expected<void, std::string>
    SocketChannel::setOption(const int connSocket, const SocketOptions opt) noexcept {
        if (connSocket < 0)
            return std::unexpected{"invalid socket descriptor (< 0)"};

        constexpr int enable = 1;
        constexpr int disable = 0;

        auto apply = [&](int level, int name, const int *val, const char *desc)
            -> std::expected<void, std::string> {
            if (::setsockopt(connSocket, level, name, val, sizeof(*val)) < 0)
                return std::unexpected{std::format("failed to set {}", desc)};
            return {};
        };

        switch (opt) {
            case SocketOptions::REUSEADDR:
                return apply(SOL_SOCKET, SO_REUSEADDR, &enable, "SO_REUSEADDR");

            case SocketOptions::SETDELAY:
                return apply(IPPROTO_TCP, TCP_NODELAY, &disable, "TCP_NODELAY=0");

            case SocketOptions::SETNODELAY:
                return apply(IPPROTO_TCP, TCP_NODELAY, &enable, "TCP_NODELAY=1");

            default:
                return std::unexpected{"unsupported socket option"};
        }
    }
}

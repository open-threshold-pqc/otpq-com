#include <stdexcept>
#include <format>
#include <ranges>
#include <utility>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <otpqcom/Sockets/SocketChannel.h>
#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetworkConfig.h>


namespace otpq::network::sockets {
    SocketChannel::SocketChannel(NodeNetConf cfg)
        : netcfg_{std::move(cfg)},
          buffer_(std::make_unique<std::uint8_t[]>(NETWORK_IO_BUFFER_SIZE)) {
        std::ranges::fill_n(buffer_.get(), NETWORK_IO_BUFFER_SIZE, std::uint8_t{0});
    }

    std::expected<void, std::string>
    SocketChannel::setOption(const int sockDesc, const helpers::SocketOptions opt) noexcept {
        if (sockDesc < 0)
            return std::unexpected{"[SocketChannel] invalid socket descriptor (<0)"};

        constexpr int enable = 1;
        constexpr int disable = 0;

        auto apply = [&](int level, int optname, const int *value, std::string_view desc)
            -> std::expected<void, std::string> {
            if (::setsockopt(sockDesc, level, optname, value, sizeof(*value)) < 0)
                return std::unexpected{std::format("failed to set {}", desc)};
            return {};
        };

        using enum helpers::SocketOptions;

        switch (opt) {
            case ReuseAddr:
                return apply(SOL_SOCKET, SO_REUSEADDR, &enable, "SO_REUSEADDR");

            case SetDelay:
                return apply(IPPROTO_TCP, TCP_NODELAY, &disable, "TCP_NODELAY=0");

            case SetNoDelay:
                return apply(IPPROTO_TCP, TCP_NODELAY, &enable, "TCP_NODELAY=1");

            default:
                return std::unexpected{"[SocketChannel] unsupported socket option"};
        }
    }
}

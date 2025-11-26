#include <arpa/inet.h>
#include <stdexcept>
#include <format>

#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network {

    NodeNetworkConfig::NodeNetworkConfig(std::string_view ip, std::uint16_t basePort)
        : ip_(ip),
          basePort_(basePort)
    {
        if (!isValidIp(ip_)) {
            throw std::invalid_argument{
                std::format("[NodeNetworkConfig]: invalid ip=\"{}\"", ip_)
            };
        }

        // basePort == 0 means "disabled / auto"
        if (!(basePort_ == 0 || basePort_ >= 1024)) {
            throw std::invalid_argument{
                std::format("[NodeNetworkConfig]: invalid base_port={} (must be >= 1024 or == 0)",
                            basePort_)
            };
        }
    }

    bool isValidIp(std::string_view ip) noexcept {
        sockaddr_in  sa4{};
        sockaddr_in6 sa6{};

        // IPv4
        if (::inet_pton(AF_INET, ip.data(), &sa4.sin_addr) == 1)
            return true;

        // IPv6
        if (::inet_pton(AF_INET6, ip.data(), &sa6.sin6_addr) == 1)
            return true;

        return false;
    }

}

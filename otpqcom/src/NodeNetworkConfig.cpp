#include <arpa/inet.h>
#include <otpqcom/NodeNetworkConfig.h>


namespace otpq::network {
    NodeNetworkConfig::NodeNetworkConfig(const int id, const std::string_view ip, const int basePort)
        : id_(id), ip_(ip), basePort_(basePort) {
        if (id_ <= 0)
            throw std::invalid_argument(
                std::format("[NodeNetworkConfig]: invalid id={} (must be > 0)", id_));

        if (isValidIp(ip_) == false)
            throw std::invalid_argument(std::format("[NodeNetworkConfig]: invalid ip={}", ip_));

        if (basePort_ < 1024 && basePort_ != 0) {
            throw std::invalid_argument(
                std::format("[NodeNetworkConfig]: invalid base_port={} (must be >= 1024)", basePort_));
        }
    }

    int NodeNetworkConfig::id() const noexcept {
        return id_;
    }

    std::string_view NodeNetworkConfig::ip() const noexcept {
        return ip_;
    }

    int NodeNetworkConfig::base_port() const noexcept {
        return basePort_;
    }

    [[nodiscard]] inline bool isValidIp(std::string_view ip) noexcept {
        sockaddr_in sa4{};
        sockaddr_in6 sa6{};
        if (::inet_pton(AF_INET, ip.data(), &sa4.sin_addr) == 1)
            return true;

        if (::inet_pton(AF_INET6, ip.data(), &sa6.sin6_addr) == 1)
            return true;

        return false;
    }
} // namespace otpq::network

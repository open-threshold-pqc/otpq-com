#include <stdexcept>
#include <format>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetHelpers.h>

namespace otpq::network {
    NodeNetworkConfig::NodeNetworkConfig(const std::string_view ip, const std::uint16_t basePort)
        : ip_(ip),
          port_(basePort) {
        if (!helpers::isValidIp(ip_))
            throw std::invalid_argument{
                std::format("[NodeNetworkConfig]: invalid ip=\"{}\"", ip_)
            };

        // basePort must be >= 1024 or == 0 (chose random port by the OS)
        if (!(port_ == 0 || port_ >= 1024))
            throw std::invalid_argument{
                std::format("[NodeNetworkConfig]: invalid base_port={} (must be >= 1024 or == 0)",
                            port_)
            };
    }
}

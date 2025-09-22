#include "../NodeNetworkConfig.h"


namespace otpq::network {
    NodeNetworkConfig::NodeNetworkConfig(const int id, const std::string_view ip, const int base_port)
        : id_(id), ip_(ip), basePort_(base_port) {
        if (id <= 0)
            throw std::invalid_argument(
                std::format("[NodeNetworkConfig]: invalid id={} (must be > 0)", id));

        // Allow 0
        if (base_port < 1024 && base_port != 0) {
            throw std::invalid_argument(
                std::format("[NodeNetworkConfig]: invalid base_port={} (must be >= 1024)", base_port));
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
} // namespace otpq::network

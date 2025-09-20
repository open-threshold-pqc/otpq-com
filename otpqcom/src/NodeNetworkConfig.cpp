#include "../NodeNetworkConfig.h"


namespace otpq::network {
    NodeNetworkConfig::NodeNetworkConfig(const int id, const std::string_view ip, const int base_port)
        : m_id(id), m_ip(ip), m_base_port(base_port) {
        if (id <= 0)
            throw std::invalid_argument(
                std::format("[NodeNetworkConfig]: invalid id={} (must be > 0)", id));

        if (base_port < 1024) {
            throw std::invalid_argument(
                std::format("[NodeNetworkConfig]: invalid base_port={} (must be >= 1024)", base_port));
        }
    }

    int NodeNetworkConfig::id() const noexcept {
        return m_id;
    }

    std::string_view NodeNetworkConfig::ip() const noexcept {
        return m_ip;
    }

    int NodeNetworkConfig::base_port() const noexcept {
        return m_base_port;
    }
} // namespace otpq::network

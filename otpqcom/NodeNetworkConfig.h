#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace otpq::network {
    /**
     * @brief Represents the network configuration of a single node.
     *
     * Stores:
     *  - An IP address (`ip`)
     *  - A base port number (`base_port`)
     *
     * Example:
     * @code
     * otpq::network::NodeNetworkConfig cfg{"127.0.0.1", 5000};
     * std::cout << cfg.ip() << ":" << cfg.base_port();
     * @endcode
     */
    class NodeNetworkConfig final {
    public:
        /// Create a new NodeNetworkConfig.
        explicit NodeNetworkConfig(std::string_view ip, std::uint16_t basePort);

        NodeNetworkConfig() = default;

        ~NodeNetworkConfig() = default;

        NodeNetworkConfig(const NodeNetworkConfig &) = default;

        NodeNetworkConfig(NodeNetworkConfig &&) noexcept = default;

        NodeNetworkConfig &operator=(const NodeNetworkConfig &) = default;

        NodeNetworkConfig &operator=(NodeNetworkConfig &&) noexcept = default;

        /// @return The node's IP address.
        [[nodiscard]] constexpr std::string_view ip() const noexcept {
            return ip_;
        }

        /// @return The node's base port.
        [[nodiscard]] constexpr std::uint16_t basePort() const noexcept {
            return basePort_;
        }

    private:
        std::string ip_{}; ///< IP address.
        std::uint16_t basePort_{}; ///< Base port (0 = disabled, or >= 1024).
    };

    /**
     * @brief Validate whether a string is an IPv4 or IPv6 address.
     *
     * Uses `inet_pton` internally and performs no allocations.
     *
     * @param ip  IP string to validate.
     * @return true if valid IPv4 or IPv6, false otherwise.
     */
    [[nodiscard]] bool isValidIp(std::string_view ip) noexcept;
}

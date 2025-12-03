#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace otpq::network {
    /**
     * @class NodeNetworkConfig
     * @brief Value-type configuration describing how a node is addressed.
     *
     * A node is described by:
     *   - an IP address (`ip`)
     *   - a base service port (`port`)
     *
     * The IP address is stored as a `std::string` and returned by value. The
     * configuration object is fully copyable and movable and exhibits standard
     * value semantics.
     *
     * Example:
     * @code
     * otpq::network::NodeNetworkConfig cfg{"::1", 9000};
     * fmt::print("{}:{}", cfg.ip(), cfg.port());
     * @endcode
     */
    class NodeNetworkConfig final {
    public:
        /**
         * @brief Construct a network configuration.
         *
         * @param ip       IPv4 or IPv6 address in text form.
         * @param basePort Base port number used by the node. A value of 0 disables
         *                 service port assignment.
         */
        explicit NodeNetworkConfig(std::string_view ip,
                                   std::uint16_t basePort = 0);

        NodeNetworkConfig() = default;

        ~NodeNetworkConfig() = default;

        NodeNetworkConfig(const NodeNetworkConfig &) = default;

        NodeNetworkConfig &operator=(const NodeNetworkConfig &) = default;

        NodeNetworkConfig(NodeNetworkConfig &&) noexcept = default;

        NodeNetworkConfig &operator=(NodeNetworkConfig &&) noexcept = default;

        /**
         * @brief Return the configured IP address by value.
         *
         * Returning by value avoids lifetime issues associated with `string_view`
         * and keeps the API simple and safe. Implementations benefit from small
         * string optimization (SSO) for short addresses.
         */
        [[nodiscard]] std::string ip() const noexcept {
            return ip_;
        }

        /**
         * @brief Return the base port associated with this node.
         */
        [[nodiscard]] std::uint16_t port() const noexcept {
            return port_;
        }

    private:
        std::string ip_{}; ///< Stored IPv4/IPv6 address in text form.
        std::uint16_t port_{}; ///< Base port for services exposed by the node.
    };

    /// Alias for convenience and readability.
    using NodeNetConf = NodeNetworkConfig;
}

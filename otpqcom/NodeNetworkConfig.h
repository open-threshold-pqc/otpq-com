#pragma once

#include <format>
#include <string>
#include <string_view>

namespace otpq::network {

    /**
     * \brief Represents the network configuration of a single node.
     *
     * This class stores basic networking information for a node in the system:
     * - a unique identifier (`id`)
     * - an IP address (`ip`)
     * - a base port number (`base_port`)
     *
     * Example:
     * \code
     * int main(){
     *  otpq::network::NodeNetworkConfig cfg(1, "127.0.0.1", 5000);
     *  std::cout << cfg.id() << " -> " << cfg.ip() << ":" << cfg.base_port();
     *}
     * \endcode
     */
    class NodeNetworkConfig {
    public:
        /**
         * \brief Construct a new NodeNetworkConfig.
         *
         * \param id Unique node identifier.
         * \param ip IP address of the node.
         * \param base_port Base port number to use for communication.
         */
        NodeNetworkConfig(int id, std::string_view ip, int base_port);

        /// @return The unique node identifier.
        [[nodiscard]] int id() const noexcept;

        /// @return The node's IP address.
        [[nodiscard]] std::string_view ip() const noexcept;

        /// @return The node's base port.
        [[nodiscard]] int base_port() const noexcept;

    private:
        int id_;             ///< Unique node identifier.
        std::string ip_;     ///< IP address of the node.
        int basePort_;      ///< Base port number.
    };

} // namespace otpq::network
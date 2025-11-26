#pragma once

#include <string>
#include <vector>
#include <span>
#include <expected>

#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network {

    /**
     * @class MPChannel
     * @brief Abstract base class for multi-peer communication.
     *
     * Provides a common interface for communication between a node and its peers.
     * Each peer is represented by a `NodeNetworkConfig`, and derived classes
     * (e.g., socket-based implementations) must implement the data transfer logic.
     *
     * Responsibilities:
     * - Send data to a specific peer.
     * - Broadcast data to all peers.
     * - Receive data from a specific peer.
     */
    class MPChannel {
    public:
        /**
         * @brief Construct a multi-peer communication context.
         *
         * @param cfg   The network configuration of this node (self).
         *
         * @note The constructor does not establish connections —
         *       it only stores configuration for later use by derived classes.
         */
        MPChannel(NodeNetworkConfig cfg);

        /// @brief Virtual destructor for safe polymorphic cleanup.
        virtual ~MPChannel() = default;

        /**
         * @brief Send a block of data to a single peer.
         *
         * @param peerId The ID of the target peer (as defined in `NodeNetworkConfig`).
         * @param data   Pointer to the raw data buffer.
         * @param len    Size of the data in bytes.
         *
         * @return std::expected<std::size_t, std::string>
         *         - On success: number of bytes sent.
         *         - On failure: error message.
         */
        virtual std::expected<std::size_t, std::string>
        sendData(int peerId, const void *data, std::size_t len) = 0;

        /**
         * @brief Broadcast a block of data to all configured peers.
         *
         * @param data Pointer to the raw data buffer.
         * @param len  Size of the data in bytes.
         *
         * @return std::expected<void, std::string>
         *         - On success: number of bytes totally broadcasted.
         *         - On failure: error message.
         */
        virtual std::expected<std::size_t, std::string>
        broadcastData(const void *data, std::size_t len) = 0;

        /**
         * @brief Receive a block of data from a specific peer.
         *
         * @param peerId The ID of the peer to receive from.
         * @param data   Pointer to the buffer where received data will be written.
         * @param len    Maximum number of bytes to read into the buffer.
         *
         * @return std::expected<std::size_t, std::string>
         *         - On success: number of bytes received.
         *         - On failure: error message.
         */
        virtual std::expected<std::size_t, std::string>
        recvData(int peerId, void *data, std::size_t len) = 0;

    protected:
        NodeNetworkConfig netcfg_;              ///< Local node configuration.
    };

} // namespace otpq::network

#pragma once

#include <string>
#include <expected>

#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network {

    /**
     * @class MPChannel
     * @brief Abstract base class for multi-peer communication.
     *
     * Represents the communication interface for a node in a multi-peer (MP)
     * environment. A node may have several peers, each identified by an integer
     * peer ID and associated with connection metadata.
     *
     * Responsibilities of derived classes:
     *   - Implement peer-to-peer send/receive
     *   - Implement broadcast semantics
     *   - Manage transport-specific state (connections, buffers, metrics...)
     *
     * This class is intentionally lightweight and stores only the local node's
     * network configuration. Derived types decide how to represent peers.
     */
    class MPChannel {
    public:
        /**
         * @brief Construct a multi-peer communication context.
         *
         * The configuration is passed by value and moved into the channel.
         * This results in:
         *   - one copy when provided an lvalue
         *   - zero copies when given an rvalue
         *
         * @param cfg Local node configuration.
         */
        explicit MPChannel(NodeNetConf cfg)
            : netcfg_{std::move(cfg)}
        {}

        /// Virtual destructor for correct cleanup through base pointer.
        virtual ~MPChannel() = default;

        /// Non-copyable: multi-peer channels often own resources.
        MPChannel(const MPChannel&) = delete;
        MPChannel& operator=(const MPChannel&) = delete;

        /// Movable: ownership can be transferred.
        MPChannel(MPChannel&&) noexcept = default;
        MPChannel& operator=(MPChannel&&) noexcept = default;

        /**
         * @brief Send data to a specific peer.
         *
         * @param peerId Identifier of the target peer.
         * @param data   Raw byte buffer.
         * @param len    Number of bytes to send.
         *
         * @return `std::expected<size_t, string>`
         *         - value: bytes successfully sent
         *         - error: failure description
         */
        [[nodiscard]]
        virtual std::expected<std::size_t, std::string>
        sendData(int peerId, const void* data, std::size_t len) = 0;

        /**
         * @brief Send a data block to all configured peers.
         *
         * @param data Raw byte buffer.
         * @param len  Length of data.
         *
         * @return `std::expected<size_t, string>`
         *         - value: total number of bytes broadcasted
         *         - error: failure description
         */
        [[nodiscard]]
        virtual std::expected<std::size_t, std::string>
        broadcastData(const void* data, std::size_t len) = 0;

        /**
         * @brief Receive a data block from a specific peer.
         *
         * @param peerId Peer from which to receive.
         * @param data   Destination buffer.
         * @param len    Max bytes to write.
         *
         * @return `std::expected<size_t, string>`
         *         - value: bytes received
         *         - error: failure description
         */
        [[nodiscard]]
        virtual std::expected<std::size_t, std::string>
        recvData(int peerId, void* data, std::size_t len) = 0;

    protected:
        /// Copy of the local node's configuration. Stable for channel lifetime.
        NodeNetConf netcfg_;
    };

}
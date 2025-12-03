#pragma once

#include <expected>
#include <string>
#include <cstdio>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetworkConfig.h>
#include <otpqcom/NetworkMetrics.h>
#include <otpqcom/Sockets/SocketChannel.h>

namespace otpq::network::sockets {

    /**
     * @class ClientSocketChannel
     * @brief TCP client-side implementation of SocketChannel.
     *
     * Represents a single outgoing TCP connection. Manages:
     *   - socket and FILE* lifetime
     *   - buffered send/receive operations
     *   - expected-based error reporting
     *
     * ClientSocketChannel is move-only and non-copyable.
     */
    class ClientSocketChannel final : public SocketChannel {
    public:
        /**
         * @brief Construct a client channel with the given network configuration.
         *
         * @param cfg Connection settings passed by value and moved into the channel.
         *
         * Does not perform the remote connection. Call nodeConnect() to establish
         * a connection to a peer.
         *
         * @throws std::runtime_error if socket creation or local setup fails.
         */
        explicit ClientSocketChannel(NodeNetConf cfg);

        /// Closes the socket and any associated FILE* stream.
        ~ClientSocketChannel() override;

        /**
         * @brief Connect to a remote endpoint.
         *
         * On success, a buffered FILE* wrapper is created for the socket.
         *
         * @param cfg Remote peer configuration.
         * @return success or an error message.
         */
        [[nodiscard]]
        std::expected<void, std::string>
        connect(const NodeNetConf& cfg) noexcept;

        /**
         * @brief Configure buffering behavior for the FILE* stream.
         *
         * Requires an active connection (stream_ must be non-null).
         */
        [[nodiscard]]
        std::expected<void, std::string>
        setBufferMode(IOBufferMode mode) const noexcept;

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        sendData(const void* data, std::size_t len) noexcept override;

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        recvData(void* data, std::size_t len) noexcept override;

        /// Flush the FILE* stream.
        [[nodiscard]]
        std::expected<void, std::string>
        streamFlush() const noexcept;

    private:
        /**
         * @brief Initialize the client-side socket.
         *
         * Applies default socket options and binds to the local address/port
         * defined in netcfg_.
         *
         * @throws std::runtime_error on failure.
         */
        void setupClient();

        FILE* stream_{nullptr};         ///< Buffered I/O stream after connect().
        int   connSocket_{-1};          ///< Socket file descriptor.
        NetworkMetrics netMetrics_;     ///< Read/write statistics.
    };

}
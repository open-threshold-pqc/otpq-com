#pragma once

#include <expected>
#include <string>
#include <cstdio>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>
#include <otpqcom/NetworkMetrics.h>

namespace otpq::network::sockets {
    /**
     * @class ClientSocketChannel
     * @brief TCP client-side implementation of SocketChannel.
     *
     * A move-only channel that owns one active connection.
     * It offers:
     *  - connect() with expected-based error reporting
     *  - buffered send/recv operations
     *  - socket and FILE* lifetime management
     */
    class ClientSocketChannel final : public SocketChannel {
    public:
        /**
         * @brief Construct a client socket channel with the given configuration.
         *
         * The constructor allocates a socket and binds to the local address.
         * Connection to a remote host occurs via nodeConnect().
         *
         * @throws std::runtime_error if socket creation or bind fails.
         */
        explicit ClientSocketChannel(NodeNetworkConfig cfg);

        /// Destructor flushes and closes the stream and socket.
        ~ClientSocketChannel() override;


        /**
         * @brief Connect to a remote node.
         *
         * On success, creates a buffered FILE* I/O stream wrapping the socket.
         *
         * @param cfg Remote endpoint configuration.
         * @return
         *   - {} on success
         *   - unexpected(error string) on failure
         */
        [[nodiscard]]
        std::expected<void, std::string>
        nodeConnect(const NodeNetworkConfig &cfg) noexcept;


        /**
         * @brief Configure buffering mode for the FILE* stream.
         *
         * Requires an established connection (stream_ must be non-null).
         */
        [[nodiscard]]
        std::expected<void, std::string>
        setBufferMode(NetworkBufferMode mode) const noexcept;

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        sendData(const void *data, std::size_t len) noexcept override;

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        recvData(void *data, std::size_t len) noexcept override;

        [[nodiscard]]
        std::expected<void, std::string>
        streamFlush() const noexcept;

    private:
        /**
         * @brief Initialize local client socket.
         *
         * Creates a TCP socket, applies default socket options,
         * and binds to the local ip/port defined by netcfg_.
         *
         * @throws std::runtime_error On creation or bind failure.
         */
        void setupClient();

        FILE *stream_{nullptr}; ///< Buffered I/O after successful connect().
        int connSocket_{-1}; ///< Active connection file descriptor.
        NetworkMetrics netMetrics_; ///< Per-connection read/write stats.
    };
} // namespace otpq::network::sockets

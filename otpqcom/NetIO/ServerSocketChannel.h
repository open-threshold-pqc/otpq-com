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
     * @class ServerSocketChannel
     * @brief TCP server-side socket wrapper.
     *
     * Responsibilities:
     *   - Create/bind/listen on a server socket
     *   - Accept one incoming connection
     *   - Provide send/recv using expected-based error reporting
     *   - Manage buffering and connection metrics
     *
     * Move-only, non-copyable. A single instance represents a single server,
     * and (after accept) one active client connection.
     */
    class ServerSocketChannel final : public SocketChannel {
    public:
        /**
         * @brief Construct a server channel with the given configuration.
         *
         * Prepares the listening socket (bind, configure).
         * Does not block or accept until awaitConnection()/acceptConnection().
         *
         * @throws std::runtime_error if setupServer() fails
         */
        explicit ServerSocketChannel(NodeNetworkConfig cfg);

        /// Destructor closes connection socket, listen socket, and FILE* stream.
        ~ServerSocketChannel() override;

        // ---------------------------------------------------------------------
        //  I/O Operations (expected-based)
        // ---------------------------------------------------------------------

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        sendData(const void *data, std::size_t len) noexcept override;

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        recvData(void *data, std::size_t len) noexcept override;

        /**
         * @brief Flush buffered output to the client.
         */
        [[nodiscard]]
        std::expected<void, std::string> streamFlush() const noexcept;

        // ---------------------------------------------------------------------
        //  Server Lifecycle
        // ---------------------------------------------------------------------

        /**
         * @brief Start listening.
         *
         * Must be called before acceptConnection().
         */
        [[nodiscard]]
        std::expected<void, std::string> awaitConnection() const noexcept;

        /**
         * @brief Accept one client.
         *
         * On success, initializes connSocket_ and stream_.
         */
        [[nodiscard]]
        std::expected<void, std::string> acceptConnection() noexcept;

        /**
         * @brief Convenience API: listen + accept.
         */
        [[nodiscard]]
        std::expected<void, std::string> awaitAndServe() noexcept;

        // ---------------------------------------------------------------------
        //  Buffering
        // ---------------------------------------------------------------------

        /**
         * @brief Configure the buffering mode for stream_.
         *
         * Error if stream_ not yet created (before acceptConnection()).
         */
        [[nodiscard]]
        std::expected<void, std::string>
        setBufferMode(NetworkBufferMode mode) const noexcept;

    private:
        /**
         * @brief Create, bind, and configure the listening socket.
         *
         * @throws std::runtime_error on failure (low-level system errors)
         */
        void setupServer();

        // ---------------------------------------------------------------------
        //  Internal State
        // ---------------------------------------------------------------------

        int listenSocket_{-1}; ///< Passive socket (listening).
        int connSocket_{-1}; ///< Active connection socket (after accept).
        FILE *stream_{nullptr}; ///< Buffered I/O wrapping connSocket_.

        NetworkMetrics netMetrics_{}; ///< Per-connection metrics.
    };
}

#pragma once

#include <expected>
#include <string>
#include <cstdio>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/Sockets/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>
#include <otpqcom/NetworkMetrics.h>

namespace otpq::network::sockets {

    /**
     * @class ServerSocketChannel
     * @brief TCP server-side socket wrapper.
     *
     * Responsibilities:
     *   - Create and configure a listening socket (bind + listen)
     *   - Accept a single incoming client connection
     *   - Provide `sendData` / `recvData` using `std::expected`
     *   - Manage optional FILE* buffering for the connected socket
     *   - Track connection-level metrics
     *
     * A `ServerSocketChannel` is move-only and non-copyable.
     * One instance represents one server endpoint and, after accept,
     * one active client connection.
     */
    class ServerSocketChannel final : public SocketChannel {
    public:

        /**
         * @brief Construct a server channel using the given configuration.
         *
         * The configuration is passed by value and then moved into the channel.
         * A listening socket is created and bound immediately.
         *
         * @param cfg Network configuration for this server instance.
         *
         * @throws std::runtime_error if socket creation or binding fails.
         */
        explicit ServerSocketChannel(NodeNetConf cfg);

        /// Close listening socket, connection socket (if present), and FILE* stream.
        ~ServerSocketChannel() override;

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        sendData(const void* data, std::size_t len) noexcept override;

        [[nodiscard]]
        std::expected<std::size_t, std::string>
        recvData(void* data, std::size_t len) noexcept override;

        /**
         * @brief Flush the buffered FILE* stream.
         *
         * @return success or a descriptive error.
         */
        [[nodiscard]]
        std::expected<void, std::string>
        streamFlush() const noexcept;

        /**
         * @brief Put the server into listening mode.
         *
         * Must be called before `acceptConnection()`.
         *
         * @return success or error (unexpected).
         */
        [[nodiscard]]
        std::expected<void, std::string>
        listen() const noexcept;

        /**
         * @brief Accept a single client connection.
         *
         * On success:
         *   - sets `connSocket_`
         *   - wraps it in a FILE* buffer (`stream_`)
         *
         * @return success or error (unexpected).
         */
        [[nodiscard]]
        std::expected<void, std::string>
        accept() noexcept;

        /**
         * @brief Convenience: listen() + accept().
         */
        [[nodiscard]]
        std::expected<void, std::string>
        listenAndAccept() noexcept;

        /**
         * @brief Set the FILE* buffering mode for the active connection.
         *
         * @param mode One of the standard C buffering modes.
         * @return success or error.
         *
         * @note Requires an established connection (`stream_ != nullptr`).
         * @note This modifies the internal buffer, so this method is NOT const.
         */
        [[nodiscard]]
        std::expected<void, std::string>
        setBufferMode(IOBufferMode mode) const noexcept;

    private:
        /**
         * @brief Create, bind, and configure the listening socket.
         *
         * @throws std::runtime_error on failure.
         */
        void setupServer();

        int listenSocket_{-1};   ///< Passive listening socket.
        int connSocket_{-1};     ///< Active connection socket after accept().
        FILE* stream_{nullptr};  ///< Buffered I/O wrapper for connSocket_.

        NetworkMetrics netMetrics_; ///< Accumulated read/write metrics.
    };

}
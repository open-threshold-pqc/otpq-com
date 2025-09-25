#pragma once

#include <expected>
#include <string>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>
#include <otpqcom/NetworkMetrics.h>

namespace otpq::network::sockets {
    /**
     * @class ClientSocketChannel
     * @brief Client-side implementation of SocketChannel.
     *
     * Provides socket communication for clients, including send/receive operations
     * and the ability to connect to a remote node. Instances are non-copyable
     * but movable, enforcing unique ownership of the underlying socket.
     */
    class ClientSocketChannel final : public SocketChannel {
    public:
        /**
         * @brief Construct a client socket channel with the given configuration.
         *        The constructor allocates the socket and binds to the given network configuration.
         * @param cfg Local network configuration (IP, port, etc.).
         * @throw std::runtime_error if socket creation or bind fails.
         */
        explicit ClientSocketChannel(NodeNetworkConfig cfg);

        /**
         * @brief Destructor.
         *
         * Flushes and closes the stream (if open) and closes the socket descriptor.
         */
        ~ClientSocketChannel() override;

        /**
         * @brief Connect to a remote node.
         *
         * Sets up a buffered FILE* stream for I/O after successful connection.
         *
         * @param cfg Remote node configuration (IP, port).
         * @return std::expected<void, std::string> Error message if connection or stream setup fails.
         */
        std::expected<void, std::string> nodeConnect(const NodeNetworkConfig &cfg);

        /**
         * @brief Configure the buffering mode of the I/O stream.
         *
         * Supported modes:
         * - NetworkBufferMode::FullyBuffered — buffer entire blocks of data
         * - NetworkBufferMode::LineBuffered  — flush on newline
         * - NetworkBufferMode::Unbuffered    — no buffering (immediate write)
         *
         * @param mode The buffering mode to apply.
         * @return std::expected<void, std::string> Error message if stream is uninitialized
         *         or if applying the buffer mode fails.
         */
        std::expected<void, std::string> setBufferMode(NetworkBufferMode mode) const;

        /**
         * @brief Send data to the connected node.
         * Retries until all bytes are sent.
         *
         * @param data Pointer to the data buffer.
         * @param len  Number of bytes to send.
         * @return std::expected<std::size_t, std::string>
         *         - On success: number of bytes sent (should equal @p len).
         *         - On failure: error message.
         */
        std::expected<std::size_t, std::string> sendData(const void *data, std::size_t len) override;

        /**
         * @brief Receive data from the connected node.
         * Retries until all requested bytes are read.
         *
         * @param data Pointer to the destination buffer.
         * @param len  Buffer size in bytes.
         * @return std::expected<std::size_t, std::string>
         *         - On success: number of bytes received (should equal @p len).
         *         - On failure: error message.
         */
        std::expected<std::size_t, std::string> recvData(void *data, std::size_t len) override;

        /**
         * @brief Flush the buffered I/O stream.
         * @return std::expected<void, std::string> Error message if flush fails.
         */
        std::expected<void, std::string> streamFlush() const;

    private:
        /**
         * @brief Initialize the client socket.
         *
         * Creates a TCP socket, applies socket options, and binds to the local address.
         * @throw std::runtime_error if socket creation or bind fails.
         */
        void setupClient();

        /// Buffered I/O stream (created after connection).
        FILE *stream_{nullptr};

        /// OS-level socket descriptor (-1 if not initialized).
        int connSocket_{-1};

        /// Per-connection network statistics (bytes sent/received).
        NetworkMetrics netMetrics_;
    };
} // namespace otpq::network::sockets

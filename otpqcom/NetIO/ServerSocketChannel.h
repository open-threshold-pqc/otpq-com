#pragma once

#include <expected>
#include <string>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>
#include <otpqcom/NetworkMetrics.h>

namespace otpq::network::sockets {

    /**
     * @class ServerSocketChannel
     * @brief Server-side socket channel implementation.
     *
     * Provides socket communication for servers, including send/receive operations
     * and the ability to receive a remote connection.
     *
     * Instances are move-only and enforce unique ownership of the underlying socket.
     */
    class ServerSocketChannel final : public SocketChannel {
    public:
        /**
         * @brief Construct a server socket channel with the given configuration.
         *        The constructor allocates and prepares the listening socket.
         * @param cfg Local network configuration (IP, port, etc.).
         * @throw std::runtime_error if socket setup fails.
         */
        explicit ServerSocketChannel(NodeNetworkConfig cfg);

        /**
         * @brief Destructor.
         *
         * Closes any active connection, the listening socket, and the I/O stream.
         */
        ~ServerSocketChannel() override;

        /**
         * @brief Send data to the connected node.
         * Retries until all bytes are sent.
         * @param data Pointer to the data buffer.
         * @param len  Number of bytes to send.
         * @return std::expected<std::size_t, std::string>
         *         - On success: number of bytes sent (should equal @p len).
         *         - On failure: error message.
         */
        std::expected<std::size_t, std::string>
        sendData(const void *data, std::size_t len) override;

        /**
         * @brief Receive data from the connected client.
         * Retries until all requested bytes are read.
         * @param data Pointer to the destination buffer.
         * @param len  Buffer size in bytes.
         * @return std::expected<std::size_t, std::string>
         *         - On success: number of bytes received (should equal @p len).
         *         - On failure: error message.
         */
        std::expected<std::size_t, std::string>
        recvData(void *data, std::size_t len) override;

        /**
         * @brief Flush the buffered I/O stream.
         * @return std::expected<void, std::string> Error message if flush fails.
         */
        std::expected<void, std::string> streamFlush() const;

        /**
         * @brief Put the server into listening mode.
         * Must be called before accepting connections.
         * @return std::expected<void, std::string> Error message if listen() fails.
         */
        std::expected<void, std::string> awaitConnection() const;

        /**
         * @brief Accept the next incoming connection.
         * Creates a dedicated client socket for communication.
         * @return std::expected<void, std::string> Error message if accept() fails.
         */
        std::expected<void, std::string> acceptConnection();

        /**
         * @brief Run the server loop: listen and accept connections.
         * Once connected, sendData() and recvData() can be used.
         * @return std::expected<void, std::string> Error message if listen() or accept() fails.
         */
        std::expected<void, std::string> awaitAndServe();

        /**
         * @brief Configure the buffering mode of the I/O stream.
         *
         * Supported modes:
         * - NetworkBufferMode::FullyBuffered — buffer entire blocks of data
         * - NetworkBufferMode::LineBuffered  — flush on newline
         * - NetworkBufferMode::Unbuffered    — no buffering (immediate write)
         *
         * @param mode The buffering mode to apply.
         * @return std::expected<void, std::string> Error message if the stream is not yet initialized
         *         or if applying the new buffer mode fails.
         */
        std::expected<void, std::string> setBufferMode(NetworkBufferMode mode) const;

    private:
        /**
         * @brief Initialize the server socket.
         * Creates, binds, and configures the listening socket.
         * @throw std::runtime_error if setup fails.
         */
        void setupServer();

        /// Listening socket descriptor (-1 if not initialized).
        int listenSocket_{-1};

        /// Buffered I/O stream (created after client acceptance).
        FILE* stream_{nullptr};

        /// Connected node socket descriptor (-1 if not connected).
        int connSocket_{-1};

        /// Per-connection network statistics (bytes sent/received).
        NetworkMetrics netMetrics_;
    };

} // namespace otpq::network::sockets

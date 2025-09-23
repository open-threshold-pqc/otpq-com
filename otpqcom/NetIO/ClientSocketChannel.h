#pragma once

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>

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
         * Retries until a connection succeeds. Once connected, sets up a
         * buffered FILE* stream for I/O.
         *
         * @param cfg Remote node configuration (IP, port).
         * @throw std::runtime_error if stream setup fails.
         */
        void nodeConnect(const NodeNetworkConfig &cfg);

        /**
         * @brief Configure the buffering mode of the I/O stream.
         *
         * Allows adjusting how data is buffered when reading/writing
         * through the underlying `FILE*` stream created after a client
         * connection is accepted.
         *
         * Supported modes:
         * - NetworkBufferMode::FullyBuffered — buffer entire blocks of data
         * - NetworkBufferMode::LineBuffered  — flush on newline
         * - NetworkBufferMode::Unbuffered    — no buffering (immediate write)
         *
         * @param mode The buffering mode to apply.
         * @throws std::runtime_error if the stream is not yet initialized or
         *         if applying the new buffer mode fails.
         *
         * @note Must be called after a connection has been accepted, since
         *       the underlying stream does not exist before that point.
         */
        void setBufferMode(NetworkBufferMode mode) const;

        /**
         * @brief Send data to the connected node.
         * Retries until all bytes are sent.
         * @param data Pointer to the data buffer.
         * @param len  Number of bytes to send.
         * @throw std::runtime_error on failure.
         */
        void sendData(const void *data, std::size_t len) override;

        /**
         * @brief Receive data from the connected node.
         * Retries until all requested bytes are read.
         * @param data Pointer to the destination buffer.
         * @param len  Buffer size in bytes.
         * @throw std::runtime_error on failure.
         */
        void recvData(void *data, std::size_t len) override;

        /**
         * @brief Flush the buffered I/O stream.
         * @throw std::runtime_error if flush fails.
         */
        void streamFlush() const;

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
    };
} // namespace otpq::network::sockets

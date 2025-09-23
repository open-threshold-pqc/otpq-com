#pragma once

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>
#include <otpqcom/NetworkConfig.h>

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
         * @throw std::runtime_error on failure.
         */
        void sendData(const void *data, std::size_t len) override;

        /**
         * @brief Receive data from the connected client.
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

        /**
         * @brief Put the server into listening mode.
         * Must be called before accepting connections.
         * @throw std::runtime_error if listen() fails.
         */
        void awaitConnection() const;

        /**
         * @brief Accept the next incoming connection.
         * Creates a dedicated client socket for communication.
         * @throw std::runtime_error if accept() fails.
         */
        void acceptConnection();

        /**
         * @brief Run the server loop: listen and accept connections.
         * Once connected, sendData() and recvData() can be used.
         * @throw std::runtime_error if listen() or accept() fails.
         */
        void awaitAndServe();

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
    };

} // namespace otpq::network::sockets

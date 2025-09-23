#pragma once

#include <cstring>
#include <memory>
#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network::sockets {

    /**
     * @enum SocketOptions
     * @brief Configurable options for a socket.
     *
     * Use with SocketChannel::setOption() to control OS-level behavior.
     */
    enum class SocketOptions {
        REUSEADDR, ///< Allow reuse of local addresses (SO_REUSEADDR).
        SETDELAY,  ///< Enable delayed sends (Nagle’s algorithm enabled).
        SETNODELAY ///< Disable delayed sends (Nagle’s algorithm disabled).
    };

    /**
     * @class SocketChannel
     * @brief Abstract base class for all socket communication channels.
     *
     * Provides the common interface for sending/receiving data and configuring sockets.
     * Classes derived from SocketChannel (e.g., ServerSocketChannel, ClientSocketChannel)
     * implement the transport-specific logic. Each socket will be handling one connection at a time.
     *
     * @note This type is move-only: copy operations are disabled, but move is supported.
     *       This enforces unique ownership of the underlying OS socket.
     */
    class SocketChannel {
    public:
        /**
         * @brief Construct a socket channel with the given configuration.
         * @param cfg Network configuration (e.g., IP address and port).
         */
        explicit SocketChannel(NodeNetworkConfig cfg);

        /// Deleted copy operations (enforces unique ownership).
        SocketChannel(const SocketChannel&) = delete;
        SocketChannel& operator=(const SocketChannel&) = delete;

        /// Defaulted move operations (transfer ownership).
        SocketChannel(SocketChannel&&) noexcept = default;
        SocketChannel& operator=(SocketChannel&&) noexcept = default;

        /**
         * @brief Virtual destructor.
         *
         * Ensures proper cleanup of resources (e.g., closing sockets).
         */
        virtual ~SocketChannel();

        /**
         * @brief Send data over the socket.
         * @param data Pointer to the bytes to send.
         * @param len  Number of bytes to send.
         * @throw std::runtime_error on failure.
         */
        virtual void sendData(const void *data, std::size_t len) = 0;

        /**
         * @brief Receive data from the socket.
         * @param data Pointer to the destination buffer.
         * @param len  Buffer size in bytes.
         * @throw std::runtime_error on failure.
         */
        virtual void recvData(void *data, std::size_t len) = 0;

    protected:
        /**
         * @brief Apply a socket option.
         * @param connSocket File descriptor of the socket.
         * @param opt Option to apply.
         *
         * @throw std::invalid_argument if @p opt is invalid.
         * @throw std::runtime_error if the system call fails.
         */
        static void setOption(int connSocket, SocketOptions opt);

        /// Network configuration for this socket (IP, port, etc.).
        NodeNetworkConfig netcfg_;

        /// Buffer for send/receive operations.
        std::unique_ptr<uint8_t[]> buffer_;
    };

} // namespace otpq::network::sockets

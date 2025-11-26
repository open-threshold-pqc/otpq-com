#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <expected>

#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network::sockets {
    /**
     * @enum SocketOptions
     * @brief OS-level socket behavior switches.
     */
    enum class SocketOptions {
        REUSEADDR, ///< Enables SO_REUSEADDR.
        SETDELAY, ///< Enables Nagle's algorithm.
        SETNODELAY ///< Disables Nagle's algorithm.
    };

    /**
     * @class SocketChannel
     * @brief Abstract base class for socket communication channels.
     *
     * Provides:
     *   - A move-only, RAII-safe socket ownership framework
     *   - A common send/recv interface for TCP communication
     *   - Utility for setting OS socket options
     *
     * Derived classes (e.g., ServerSocketChannel, ClientSocketChannel)
     * must implement transport-specific logic.
     *
     * This class represents **one connection per object**.
     */
    class SocketChannel {
    public:
        /// Construct a socket channel tied to a network configuration.
        explicit SocketChannel(NodeNetworkConfig cfg);

        /// Deleted copy (sockets cannot be shared).
        SocketChannel(const SocketChannel &) = delete;

        SocketChannel &operator=(const SocketChannel &) = delete;

        /// Move-enabled (ownership transfer).
        SocketChannel(SocketChannel &&) noexcept = default;

        SocketChannel &operator=(SocketChannel &&) noexcept = default;

        /// Virtual destructor ensures correct cleanup by derived classes.
        virtual ~SocketChannel();

        /**
         * @brief Send bytes over the socket reliably.
         *
         * @param data Pointer to the buffer to send.
         * @param len  Number of bytes to send.
         *
         * @return
         *   - `std::expected<size_t, std::string>`
         *   - On success → number of bytes sent (normally == len)
         *   - On failure → error message
         *
         * @note This is a high-level API returning errors via `expected`.
         */
        [[nodiscard]]
        virtual std::expected<std::size_t, std::string>
        sendData(const void *data, std::size_t len) = 0;

        /**
         * @brief Receive bytes from the socket reliably.
         *
         * @param data Destination buffer.
         * @param len  Maximum number of bytes to read.
         *
         * @return
         *   - On success → bytes read (normally == len)
         *   - On failure → error message
         */
        [[nodiscard]]
        virtual std::expected<std::size_t, std::string>
        recvData(void *data, std::size_t len) = 0;

    protected:
        /**
         * @brief Apply a low-level socket option.
         *
         * @param connSocket The socket file descriptor.
         * @param opt        Socket option to enable.
         *
         * @throws std::invalid_argument On invalid option.
         * @throws std::runtime_error   On system call failure.
         *
         * @note This function uses exceptions intentionally since it is
         *       a low-level helper, not part of the public `expected` API.
         */
        [[nodiscard]]
        static std::expected<void, std::string>
        setOption(int connSocket, SocketOptions opt) noexcept;

        /// Network configuration for this socket.
        NodeNetworkConfig netcfg_;

        /// Intermediate buffer for I/O operations.
        std::unique_ptr<std::uint8_t[]> buffer_;
    };
}

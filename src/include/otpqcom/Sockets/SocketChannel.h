#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <expected>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetHelpers.h>


namespace otpq::network::sockets {
    /**
     * @class SocketChannel
     * @brief Abstract base class representing a single socket connection.
     *
     * `SocketChannel` provides:
     *   - RAII ownership of a single socket file descriptor
     *   - Move-only semantics (sockets cannot be copied or shared)
     *   - A uniform high-level send/receive interface using `std::expected`
     *   - Helpers for applying OS-level socket options
     *
     * Derived types implement transport-specific behavior (e.g. client/server
     * semantics, connection establishment, shutdown logic, etc.).
     *
     * ## Ownership model
     *
     * A `SocketChannel` instance exclusively owns its connection. The
     * underlying file descriptor is released on destruction. Copying is
     * prohibited; move semantics are enabled for transfer of ownership.
     *
     * ## Network configuration lifetime
     *
     * `SocketChannel` always stores its own copy of `NodeNetConf`. Passing a
     * reference, temporary, or rvalue all result in an internal copy.
     */
    class SocketChannel {
    public:
        /**
         * @brief Construct a socket channel using a network configuration.
         *
         * The network configuration is passed by value and then moved into the channel.
         * This results in:
         *   - One copy when the caller provides an lvalue
         *   - No copies (only a move) when the caller provides an rvalue
         *
         * @param cfg Network configuration to store in the channel.
         */
        explicit SocketChannel(NodeNetConf cfg);

        /// Move constructor — transfers channel ownership.
        SocketChannel(SocketChannel&&) noexcept = default;

        /// Move assignment — transfers channel ownership.
        SocketChannel& operator=(SocketChannel&&) noexcept = default;

        /// Copying is prohibited — socket ownership cannot be duplicated.
        SocketChannel(const SocketChannel&) = delete;
        SocketChannel& operator=(const SocketChannel&) = delete;

        /// Virtual destructor for correct cleanup of derived types.
        virtual ~SocketChannel() = default;

        /**
         * @brief Send raw bytes to the remote peer.
         *
         * Implementations must attempt to send exactly @p len bytes unless an
         * error occurs. Partial sends should be surfaced as success with the
         * actual number of bytes sent.
         *
         * @param data Pointer to the buffer containing the data.
         * @param len  Number of bytes to send.
         *
         * @return `std::expected<size_t, std::string>`
         *   - `value`   → number of bytes sent
         *   - `error`   → descriptive error message
         */
        [[nodiscard]]
        virtual std::expected<std::size_t, std::string>
        sendData(const void* data, std::size_t len) = 0;

        /**
         * @brief Receive raw bytes from the remote peer.
         *
         * Implementations should read up to @p len bytes, blocking or
         * non-blocking depending on the socket mode.
         *
         * @param data Destination buffer.
         * @param len  Maximum number of bytes to read.
         *
         * @return `std::expected<size_t, std::string>`
         *   - `value`   → number of bytes received
         *   - `error`   → descriptive error message
         */
        [[nodiscard]]
        virtual std::expected<std::size_t, std::string>
        recvData(void* data, std::size_t len) = 0;

    protected:
        /**
         * @brief Apply a platform-specific socket option.
         *
         * @param sockDesc   Socket file descriptor.
         * @param opt  Option to apply.
         *
         * @return
         *   - `std::expected<void, std::string>{}` on success
         *   - `std::unexpected(std::string)` on failure
         *
         * @note This function uses error-based reporting via `std::expected`
         *       rather than exceptions. Derived classes may wrap errors in
         *       higher-level logic as needed.
         */
        [[nodiscard]]
        static std::expected<void, std::string>
        setOption(int sockDesc, helpers::SocketOptions opt) noexcept;

        /// Per-instance copy of the network configuration.
        NodeNetConf netcfg_{};

        /// Internal I/O scratch buffer (resizable by derived classes).
        std::unique_ptr<std::uint8_t[]> buffer_{};
    };

}
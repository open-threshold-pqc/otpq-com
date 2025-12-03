#pragma once

#include <string_view>

namespace otpq::network::helpers {
    /**
     * @brief Check whether a string is a syntactically valid IPv4 or IPv6 address.
     *
     * This function performs a strict syntax check using `inet_pton`. It performs
     * no dynamic allocation and returns immediately on malformed input.
     *
     * @param ip Candidate IP address in textual form.
     * @return true if `ip` is a valid IPv4 or IPv6 address; false otherwise.
     */
    [[nodiscard]] bool isValidIp(std::string_view ip) noexcept;

    /**
     * @brief Socket configuration flags controlling OS-level behavior.
     *
     * These map directly to platform socket options such as SO_REUSEADDR
     * or TCP_NODELAY. Options may silently no-op on unsupported platforms.
     */
    enum class SocketOptions {
        ReuseAddr, ///< Enable SO_REUSEADDR.
        SetDelay, ///< Enable Nagle's algorithm (TCP_DELAY).
        SetNoDelay ///< Disable Nagle's algorithm (TCP_NODELAY).
    };
}

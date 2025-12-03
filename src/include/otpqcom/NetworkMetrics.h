#pragma once

#include <cstdint>


namespace otpq::network {
    /**
     * @struct NetworkMetrics
     * @brief Tracks basic network I/O statistics for a socket or channel.
     *
     * This structure provides counters for the total number of bytes
     * sent and received over a connection. It is designed to be lightweight
     * and easily extended if more metrics (e.g., packet counts, error counts)
     * are needed in the future.
     */
    struct NetworkMetrics {
        /// Total number of bytes successfully sent.
        std::uint64_t bytesSent{0};

        /// Total number of bytes successfully received.
        std::uint64_t bytesReceived{0};

        /**
         * @brief Reset all metrics to zero.
         */
        constexpr void reset() noexcept {
            bytesSent = 0;
            bytesReceived = 0;
        }
    };
}

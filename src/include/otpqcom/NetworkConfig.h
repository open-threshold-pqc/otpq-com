#pragma once

#include <cstddef>
#include <stdio.h>

namespace otpq::network {
    /**
     * @brief Buffering strategies for network I/O.
     *
     * These map directly to the underlying C stdio buffering modes:
     * - FullyBuffered → _IOFBF (full block buffering)
     * - LineBuffered  → _IOLBF (flushes on newline)
     * - Unbuffered    → _IONBF (no buffering)
     */
    enum class IOBufferMode : int {
        FullyBuffered = _IOFBF,
        LineBuffered = _IOLBF,
        Unbuffered = _IONBF
    };

    /**
     * @brief Default I/O buffer size in bytes.
     *
     * The default is 1 MiB.
     * This value can be overridden at compile time using:
     *
     *     -DNETWORK_BUFFER_SIZE=<bytes>
     */
#ifndef NETWORK_BUFFER_SIZE
    inline constexpr std::size_t NETWORK_IO_BUFFER_SIZE = 1024 * 1024;
#else
    inline constexpr std::size_t NETWORK_IO_BUFFER_SIZE = NETWORK_BUFFER_SIZE;
#endif


    /**
     * @brief Selects the default buffering mode for network I/O.
     *
     * This setting can be overridden at compile time by defining:
     *
     *     -DNETWORK_BUFFER_MODE=<Token>
     *
     * where <Token> must be one of:
     *     FullyBuffered
     *     LineBuffered
     *     Unbuffered
     *
     * Example:
     *
     *     g++ -DNETWORK_BUFFER_MODE=Unbuffered ...
     */
#ifndef NETWORK_BUFFER_MODE
#define NETWORK_BUFFER_MODE FullyBuffered
#endif

    // Convert user token → enum value
#define OTPQ_MATCH_MODE(token, mode) \
    inline constexpr IOBufferMode NETWORK_IO_BUFFER_MODE = mode;

#if NETWORK_BUFFER_MODE == FullyBuffered
    OTPQ_MATCH_MODE(FullyBuffered, IOBufferMode::FullyBuffered)
#elif NETWORK_BUFFER_MODE == LineBuffered
    OTPQ_MATCH_MODE(LineBuffered, IOBufferMode::LineBuffered)
#elif NETWORK_BUFFER_MODE == Unbuffered
    OTPQ_MATCH_MODE(Unbuffered, IOBufferMode::Unbuffered)
#else
    static_assert(false,
                  "Invalid NETWORK_BUFFER_MODE. Allowed values: "
                  "FullyBuffered, LineBuffered, Unbuffered."
    );
#endif

#undef OTPQ_MATCH_MODE
}

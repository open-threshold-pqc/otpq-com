#pragma once

#include <cstddef>
#include <clocale>

namespace otpq::network {
    /**
     * @brief Buffering modes for network I/O streams.
     *
     * These map directly to the standard C library's buffering modes:
     * - FullyBuffered (`_IOFBF`)
     * - LineBuffered  (`_IOLBF`)
     * - Unbuffered    (`_IONBF`)
     */
    enum class NetworkBufferMode : int {
        FullyBuffered = _IOFBF, ///< Buffer entire blocks of data.
        LineBuffered = _IOLBF, ///< Flush on newline.
        Unbuffered = _IONBF ///< No buffering (write immediately).
    };

    /**
     * @brief Default buffer size for network I/O (1 MB).
     *
     * Can be overridden at compile time with `-DNETWORK_BUFFER_SIZE=...`.
     */
#ifndef NETWORK_BUFFER_SIZE
    inline constexpr std::size_t NETWORK_IO_BUFFER_SIZE = 1024 * 1024;
#else
    inline constexpr std::size_t NETWORK_IO_BUFFER_SIZE = NETWORK_BUFFER_SIZE;
#endif


    /**
     * @brief Default buffering mode for network I/O streams.
     *
     * Can be overridden at compile time with `-DNETWORK_BUFFER_MODE=<value>`.
     * Allowed values:
     *   - 0 → FullyBuffered
     *   - 1 → LineBuffered
     *   - 2 → Unbuffered
     */
    /**
     * @brief Default buffering mode for network I/O streams.
     *
     * Can be overridden at compile time with:
     *   -DNETWORK_BUFFER_MODE=FullyBuffered
     *   -DNETWORK_BUFFER_MODE=LineBuffered
     *   -DNETWORK_BUFFER_MODE=Unbuffered
     */
#ifndef NETWORK_BUFFER_MODE
    inline constexpr auto NETWORK_IO_BUFFER_MODE = NetworkBufferMode::FullyBuffered;
#else
#if NETWORK_BUFFER_MODE == FullyBuffered
    inline constexpr auto NETWORK_IO_BUFFER_MODE = NetworkBufferMode::FullyBuffered;
#elif NETWORK_BUFFER_MODE == LineBuffered
    inline constexpr auto NETWORK_IO_BUFFER_MODE = NetworkBufferMode::LineBuffered;
#elif NETWORK_BUFFER_MODE == Unbuffered
    inline constexpr auto NETWORK_IO_BUFFER_MODE = NetworkBufferMode::Unbuffered;
#else
#error "Invalid NETWORK_BUFFER_MODE. Use FullyBuffered, LineBuffered, or Unbuffered."
#endif
#endif

} // namespace otpq::network

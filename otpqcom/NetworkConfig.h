#pragma once

#include <cstddef>

namespace otpq::network {
    enum class NetworkBufferMode : int {
        FullyBuffered = _IOFBF,
        LineBuffered = _IOLBF,
        Unbuffered = _IONBF
    };


#ifndef NETWORK_BUFFER_SIZE
#define NETWORK_BUFFER_SIZE 1024 * 1024
#endif

// #ifndef NETWORK_BUFFER_MODE
// #define NETWORK_BUFFER_MODE NetworkBufferMode::FullyBuffered
// #endif


    constexpr std::size_t NETWORK_IO_BUFFER_SIZE = NETWORK_BUFFER_SIZE;
    // constexpr auto NETWORK_IO_BUFFER_MODE = NETWORK_BUFFER_MODE;

}

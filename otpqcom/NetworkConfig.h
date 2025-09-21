#pragma once
#include <cstddef>
#include <stdio.h>

namespace otpq::network {
    constexpr std::size_t NETWORK_BUFFER_SIZE = 1024 * 1024; // 1 MB
    enum class NetworkBufferMode : int {
        FullyBuffered = _IOFBF,
        LineBuffered = _IOLBF,
        Unbuffered = _IONBF
    };
}

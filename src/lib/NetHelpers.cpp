#include <string_view>
#include <arpa/inet.h>

namespace otpq::network::helpers {
    bool isValidIp(const std::string_view ip) noexcept {
        sockaddr_in sa4{};
        sockaddr_in6 sa6{};

        const char *raw = ip.data();

        return (::inet_pton(AF_INET, raw, &sa4.sin_addr) == 1)
               || (::inet_pton(AF_INET6, raw, &sa6.sin6_addr) == 1);
    }
}


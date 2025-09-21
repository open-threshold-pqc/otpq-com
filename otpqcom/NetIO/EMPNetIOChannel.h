#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <iostream>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetworkConfig.h>

namespace emp::network {
    enum class EMPNetworkNodeType {
        SERVER,
        CLIENT
    };

    class EMPNetIOChannel {
    public:
        explicit EMPNetIOChannel(otpq::network::NodeNetworkConfig netcfg,
                                 EMPNetworkNodeType nodeType = EMPNetworkNodeType::CLIENT);

        ~EMPNetIOChannel();

        void nodeListen();

        void sync();

        void set_no_delay() const;

        void set_delay() const;

        void flush() const;

        void send_data_internal(const void *data, std::size_t len);

        void recv_data_internal(void *data, std::size_t len);

    private:
        void setup_server();

        void setup_client();

        otpq::network::NodeNetworkConfig netcfg_;

        EMPNetworkNodeType nodeType_{};
        int connSocket_{-1};
        FILE *stream_{};
        std::vector<char> buffer_{};
        bool hasSent_{};
    };
} // namespace emp::network

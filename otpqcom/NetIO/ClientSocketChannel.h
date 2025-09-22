#pragma once

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>

namespace otpq::network::sockets {
    class ClientSocketChannel final : public SocketChannel {
    public:
        explicit ClientSocketChannel(NodeNetworkConfig cfg);

        ~ClientSocketChannel();

        void sendData(const void *data, std::size_t len) override;

        void recvData(void *data, std::size_t len) override;

        void nodeConnect(const NodeNetworkConfig &cfg);

        void setOption(SocketOptions opt) const;

    private:
        void setupClient();

        int connSocket_{-1};
        bool hasSent_{false};
    };
} // namespace otpq::network

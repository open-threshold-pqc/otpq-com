#pragma once
#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/SocketChannel.h>

namespace otpq::network::sockets {
    class ServerSocketChannel final : public SocketChannel {
    public:
        explicit ServerSocketChannel(NodeNetworkConfig cfg);

        ~ServerSocketChannel();

        void sendData(const void *data, std::size_t len) override;

        void recvData(void *data, std::size_t len) override;

        void beginListen();

        void setOption(SocketOptions opt) const;

    private:
        void setupServer();

        int connSocket_{-1};
        bool hasSent_{false};
    };
} // namespace otpq::network

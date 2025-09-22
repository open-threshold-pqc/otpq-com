#pragma once

#include <cstring>
#include <memory>
#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetworkConfig.h>

namespace otpq::network::sockets {

    enum class SocketOptions {
        REUSEADDR,
        SETDELAY,
        SETNODELAY,
    };

    class SocketChannel {
    public:
        explicit SocketChannel(NodeNetworkConfig cfg)
            : netcfg_(std::move(cfg)),
              buffer_(std::make_unique<uint8_t[]>(NETWORK_IO_BUFFER_SIZE)) {

            std::memset(buffer_.get(), 0, NETWORK_IO_BUFFER_SIZE);
        }

        virtual ~SocketChannel() = default;

        virtual void sendData(const void *data, std::size_t len) = 0;

        virtual void recvData(void *data, std::size_t len) = 0;

    protected:
        NodeNetworkConfig netcfg_;
        std::unique_ptr<uint8_t[]> buffer_;
        FILE *stream_{};
    };
} // namespace otpq::network

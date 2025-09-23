#include <iostream>
#include <string>
#include <array>
#include <chrono>

#include <otpqcom/NodeNetworkConfig.h>
#include "otpqcom/NetIO/ClientSocketChannel.h"

using namespace otpq::network;

int main() {
    const NodeNetworkConfig clientCfg{2, "127.0.0.1", 10000};
    const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};

    sockets::ClientSocketChannel client{clientCfg};
    client.nodeConnect(serverCfg);

    std::cout << "[Client] Connected to server at "
              << serverCfg.ip() << ":" << serverCfg.base_port() << '\n';

    // Prepare message
    const std::string message = "HELLO";
    client.sendData(message.data(), message.size());
    client.streamFlush();

    // Receive response
    std::array<char, 6> buffer{};
    client.recvData(buffer.data(), message.size());
    buffer[message.size()] = '\0';

    std::cout << "[Client] Received reply: " << buffer.data() << '\n';

    return 0;
}

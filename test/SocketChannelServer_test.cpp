#include <iostream>
#include <string>
#include <array>

#include <otpqcom/NodeNetworkConfig.h>
#include "otpqcom/NetIO/ServerSocketChannel.h"

using namespace otpq::network;

int main() {
    const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};
    sockets::ServerSocketChannel server{serverCfg};

    std::cout << "[Server] Listening on "
              << serverCfg.ip() << ":" << serverCfg.base_port() << '\n';

    server.awaitAndServe();
    std::cout << "[Server] Client connected\n";

    // Receive data
    std::array<char, 6> buffer{};
    server.recvData(buffer.data(), 5);
    buffer[5] = '\0';

    std::cout << "[Server] Received: " << buffer.data() << '\n';

    // Optional: Send reply
    const std::string reply = "THANK";
    server.sendData(reply.data(), reply.size());
    server.streamFlush();
    std::cout << "[Server] Sent reply: " << reply << '\n';

    return 0;
}

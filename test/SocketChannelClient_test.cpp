#include <iostream>
#include <string>
#include <array>
#include <chrono>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/ClientSocketChannel.h>

using namespace otpq::network;

int main() {
    const NodeNetworkConfig clientCfg{2, "127.0.0.1", 10000};
    const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};

    sockets::ClientSocketChannel client{clientCfg};

    // Connect
    if (auto res = client.nodeConnect(serverCfg); !res) {
        std::cerr << res.error() << "\n";
        return 1;
    }

    std::cout << "Connected to server at "
              << serverCfg.ip() << ":" << serverCfg.base_port() << '\n';

    // Prepare message
    const std::string message = "HELLO";

    // Send
    if (auto sent = client.sendData(message.data(), message.size()); !sent) {
        std::cerr << sent.error() << "\n";
        return 1;
    } else {
        std::cout << "Sent " << *sent << " bytes\n";
    }

    if (auto res = client.streamFlush(); !res) {
        std::cerr << res.error() << "\n";
        return 1;
    }

    // Receive response
    std::array<char, 6> buffer{};
    if (auto received = client.recvData(buffer.data(), message.size()); !received) {
        std::cerr << received.error() << "\n";
        return 1;
    } else {
        buffer[*received] = '\0'; // ensure null termination
        std::cout << "Received reply: " << buffer.data() << '\n';
    }

    return 0;
}

#include <iostream>
#include <string>
#include <array>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/ServerSocketChannel.h>

using namespace otpq::network;

int main() {
    const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};
    sockets::ServerSocketChannel server{serverCfg};

    std::cout << "Listening on "
              << serverCfg.ip() << ":" << serverCfg.base_port() << '\n';

    if (auto res = server.awaitAndServe(); !res) {
        std::cerr << res.error() << "\n";
        return 1;
    }
    std::cout << "Client connected\n";

    // Receive data
    std::array<char, 6> buffer{};
    if (auto received = server.recvData(buffer.data(), 5); !received) {
        std::cerr << received.error() << "\n";
        return 1;
    } else {
        buffer[*received] = '\0';
        std::cout << "Received: " << buffer.data() << '\n';
    }

    // Send reply
    const std::string reply = "THANK";
    if (auto sent = server.sendData(reply.data(), reply.size()); !sent) {
        std::cerr << sent.error() << "\n";
        return 1;
    } else {
        std::cout << "Sent " << *sent << " bytes\n";
    }

    if (auto res = server.streamFlush(); !res) {
        std::cerr << res.error() << "\n";
        return 1;
    }

    std::cout << "Sent reply: " << reply << '\n';

    return 0;
}

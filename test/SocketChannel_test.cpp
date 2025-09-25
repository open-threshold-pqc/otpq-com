#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <chrono>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/ClientSocketChannel.h>
#include <otpqcom/NetIO/ServerSocketChannel.h>

using namespace otpq::network;

void runServer() {
    const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};
    sockets::ServerSocketChannel server{serverCfg};

    std::cout << "Listening on " << serverCfg.ip() << ":" << serverCfg.base_port() << '\n';

    if (auto res = server.awaitAndServe(); !res) {
        std::cerr << res.error() << "\n";
        return;
    }
    std::cout << "Client connected\n";

    // Receive data
    std::array<char, 6> buffer{};
    if (auto received = server.recvData(buffer.data(), 5); !received) {
        std::cerr << received.error() << "\n";
        return;
    } else {
        buffer[*received] = '\0';
        std::cout << "Received: " << buffer.data() << '\n';
    }

    // Send reply
    const std::string reply = "THANK";
    if (auto sent = server.sendData(reply.data(), reply.size()); !sent) {
        std::cerr << sent.error() << "\n";
        return;
    } else {
        std::cout << "Sent " << *sent << " bytes\n";
    }

    if (auto res = server.streamFlush(); !res) {
        std::cerr << res.error() << "\n";
        return;
    }

    std::cout << "Sent reply: " << reply << '\n';
}

void runClient() {
    // give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds{100});

    const NodeNetworkConfig clientCfg{2, "127.0.0.1", 10000};
    const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};

    sockets::ClientSocketChannel client{clientCfg};

    if (auto res = client.nodeConnect(serverCfg); !res) {
        std::cerr << res.error() << "\n";
        return;
    }

    std::cout << "Connected to server at " << serverCfg.ip() << ":" << serverCfg.base_port() << '\n';

    const std::string message = "HELLO";

    // Send
    if (auto sent = client.sendData(message.data(), message.size()); !sent) {
        std::cerr << sent.error() << "\n";
        return;
    } else {
        std::cout << "Sent " << *sent << " bytes\n";
    }

    if (auto res = client.streamFlush(); !res) {
        std::cerr << res.error() << "\n";
        return;
    }

    // Receive response
    std::array<char, 6> buffer{};
    if (auto received = client.recvData(buffer.data(), message.size()); !received) {
        std::cerr << received.error() << "\n";
        return;
    } else {
        buffer[*received] = '\0';
        std::cout << "Received reply: " << buffer.data() << '\n';
    }
}

int main() {
    std::thread serverThread(runServer);
    std::thread clientThread(runClient);

    serverThread.join();
    clientThread.join();

    return 0;
}

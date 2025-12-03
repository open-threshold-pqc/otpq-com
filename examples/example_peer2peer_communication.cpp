#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/Sockets/ClientSocketChannel.h>
#include <otpqcom/Sockets/ServerSocketChannel.h>

using namespace otpq::network;

constexpr int RANDOM_BROADCAST_MESSAGE_SIZE = 50;
constexpr int REPLY_MESSAGE_SIZE = 10;

std::string randomMessage() {
    static thread_local std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist('A', 'Z');
    std::string s;
    s.reserve(RANDOM_BROADCAST_MESSAGE_SIZE);
    for (int i = 0; i < RANDOM_BROADCAST_MESSAGE_SIZE; ++i)
        s.push_back(static_cast<char>(dist(rng)));
    return s;
}

void runServer() {
    const NodeNetConf servingConfig{"127.0.0.1", 9000};
    sockets::ServerSocketChannel server{servingConfig};

    std::cout << "[SERVER] Listening on " << servingConfig.ip() << ":" << servingConfig.port() << '\n';

    if (auto r0 = server.listenAndAccept(); !r0) {
        std::cerr << r0.error() << '\n';
        return;
    }

    std::vector<char> buffer(RANDOM_BROADCAST_MESSAGE_SIZE);
    if (auto r2 = server.recvData(buffer.data(), buffer.size()); !r2) {
        std::cerr << r2.error() << '\n';
        return;
    }

    const std::string received(buffer.begin(), buffer.end());
    std::cout << "[SERVER] Received: " << received << '\n';

    const std::string reply{"Server ACK"};
    if (auto r3 = server.sendData(reply.data(), reply.size()); !r3) {
        std::cerr << r3.error() << '\n';
        return;
    }

    if (auto r4 = server.streamFlush(); !r4) {
        std::cerr << r4.error() << '\n';
        return;
    }

    std::cout << "[SERVER] Reply sent\n";
}

void runClient() {
    sockets::ClientSocketChannel sock{NodeNetConf{"127.0.0.1"}};
    NodeNetConf serverAddr{"127.0.0.1", 9000};

    if (auto c0 = sock.connect(serverAddr); !c0) {
        std::cerr << c0.error() << '\n';
        return;
    }

    std::cout << "[CLIENT] Connected to the Server\n";

    const std::string message { randomMessage()};
    std::cout << "[CLIENT] Sending a random message: " << message << '\n';

    if (auto s1 = sock.sendData(message.data(), message.size()); !s1) {
        std::cerr << s1.error() << '\n';
        return;
    }

    if (auto f0 = sock.streamFlush(); !f0) {
        std::cerr << f0.error() << '\n';
        return;
    }

    std::vector<uint8_t> buffer(REPLY_MESSAGE_SIZE);
    auto r1 = sock.recvData(buffer.data(), REPLY_MESSAGE_SIZE);
    if (!r1) {
        std::cerr << r1.error() << '\n';
        return;
    }

    const std::string reply(buffer.data(), buffer.data() + *r1);
    std::cout << "[CLIENT] Received reply: " << reply << "\n";
}

int main() {
    std::thread serverThread(runServer);
    std::thread clientThread(runClient);

    serverThread.join();
    clientThread.join();
}

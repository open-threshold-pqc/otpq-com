#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/NetIO/ClientSocketChannel.h>
#include <otpqcom/NetIO/ServerSocketChannel.h>

using namespace otpq::network;


constexpr int CLIENT_MESSAGE_SIZE = 50;
constexpr int REPLY_MESSAGE_SIZE = 5;

std::string randomMessage() {
    static thread_local std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist('A', 'Z');
    std::string s;
    s.reserve(CLIENT_MESSAGE_SIZE);
    for (int i = 0; i < CLIENT_MESSAGE_SIZE; ++i)
        s.push_back(static_cast<char>(dist(rng)));
    return s;
}

void runServer() {
    const NodeNetworkConfig cfg{"127.0.0.1", 9000};
    sockets::ServerSocketChannel server{cfg};

    std::cout << "[SERVER] Listening on " << cfg.ip() << ":" << cfg.basePort() << '\n';

    if (auto r0 = server.awaitAndServe(); !r0) {
        std::cerr << r0.error() << '\n';
        return;
    }

    std::vector<char> buffer(CLIENT_MESSAGE_SIZE);
    if (auto r2 = server.recvData(buffer.data(), buffer.size()); !r2) {
        std::cerr << r2.error() << '\n';
        return;
    }

    const std::string received(buffer.begin(), buffer.end());
    std::cout << "[SERVER] Received: " << received << '\n';

    const std::string reply {"ACK50"};
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
    sockets::ClientSocketChannel client{{"127.0.0.1"}};

    if (auto c0 = client.nodeConnect({"127.0.0.1", 9000}); !c0) {
        std::cerr << c0.error() << '\n';
        return;
    }

    std::cout << "[CLIENT] Connected\n";

    std::string message = randomMessage();
    std::cout << "[CLIENT] Sending message: " << message << '\n';

    if (auto s1 = client.sendData(message.data(), message.size()); !s1) {
        std::cerr << s1.error() << '\n';
        return;
    }

    if (auto f0 = client.streamFlush(); !f0) {
        std::cerr << f0.error() << '\n';
        return;
    }

    std::vector<char> buffer(REPLY_MESSAGE_SIZE);
    auto r1 = client.recvData(buffer.data(), REPLY_MESSAGE_SIZE);
    if (!r1) {
        std::cerr << r1.error() << '\n';
        return;
    }

    std::string reply(buffer.data(), buffer.data() + *r1);
    std::cout << "[CLIENT] Received reply: " << reply << "\n";
}

int main() {
    std::thread serverThread(runServer);
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    std::thread clientThread(runClient);

    serverThread.join();
    clientThread.join();
}

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include <otpqcom/NodeNetworkConfig.h>
#include <otpqcom/Sockets/ClientSocketChannel.h>
#include <otpqcom/Sockets/ServerSocketChannel.h>

#include <gtest/gtest.h>

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

void runServer(std::string& receivedOut, bool& serverOk) {
    serverOk = false;

    const NodeNetworkConfig cfg{"127.0.0.1", 9000};
    sockets::ServerSocketChannel server{cfg};

    auto r0 = server.listenAndAccept();
    if (!r0) return;

    std::vector<char> buffer(RANDOM_BROADCAST_MESSAGE_SIZE);
    auto r2 = server.recvData(buffer.data(), buffer.size());
    if (!r2) return;

    receivedOut.assign(buffer.begin(), buffer.end());

    const std::string reply{"Server ACK"};
    auto r3 = server.sendData(reply.data(), reply.size());
    if (!r3) return;

    auto r4 = server.streamFlush();
    if (!r4) return;

    serverOk = true;
}

void runClient(std::string& sentOut, std::string& replyOut, bool& clientOk) {
    clientOk = false;

    sockets::ClientSocketChannel client{NodeNetworkConfig{"127.0.0.1"}};

    auto c0 = client.connect(NodeNetworkConfig{"127.0.0.1", 9000});
    if (!c0) return;

    sentOut = randomMessage();

    auto s1 = client.sendData(sentOut.data(), sentOut.size());
    if (!s1) return;

    auto f0 = client.streamFlush();
    if (!f0) return;

    std::vector<char> buffer(REPLY_MESSAGE_SIZE);
    auto r1 = client.recvData(buffer.data(), REPLY_MESSAGE_SIZE);
    if (!r1) return;

    replyOut.assign(buffer.data(), buffer.data() + *r1);

    clientOk = true;
}

TEST(NetworkSocketTest, ClientServerMessageExchange)
{
    std::string receivedByServer;
    std::string sentByClient;
    std::string replyToClient;

    bool serverOk = false;
    bool clientOk = false;

    std::thread serverThread([&]() { runServer(receivedByServer, serverOk); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::thread clientThread([&]() { runClient(sentByClient, replyToClient, clientOk); });

    serverThread.join();
    clientThread.join();

    ASSERT_TRUE(serverOk) << "Server failed before completing data exchange.";
    ASSERT_TRUE(clientOk) << "Client failed before completing data exchange.";

    EXPECT_EQ(receivedByServer.size(), RANDOM_BROADCAST_MESSAGE_SIZE);
    EXPECT_EQ(sentByClient.size(), RANDOM_BROADCAST_MESSAGE_SIZE);
    EXPECT_EQ(receivedByServer, sentByClient);

    EXPECT_EQ(replyToClient, "Server ACK");
}

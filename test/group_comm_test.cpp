#include <gtest/gtest.h>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

#include <otpqcom/Sockets/SocketMPChannel.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

constexpr int RANDOM_BROADCAST_MESSAGE_SIZE = 50;
constexpr int REPLY_MESSAGE_SIZE = 50;

std::string randomMessage() {
    static thread_local std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist('A', 'Z');
    std::string s;
    s.reserve(RANDOM_BROADCAST_MESSAGE_SIZE);
    for (int i = 0; i < RANDOM_BROADCAST_MESSAGE_SIZE; ++i)
        s.push_back(static_cast<char>(dist(rng)));
    return s;
}

struct NodeResult {
    bool ok = false;
    std::string received; // only used for nodes 2–4
};

void runNode(
    int selfId,
    NodeNetworkConfig selfCfg,
    std::span<std::pair<int, NodeNetworkConfig> > peers,
    const std::string *broadcastMsg,
    NodeResult *result) {
    SocketMPChannel comm{selfId, selfCfg, peers};
    result->ok = false;

    // Sender node (1)
    if (selfId == 1) {
        auto res = comm.broadcastData(broadcastMsg->data(), broadcastMsg->size());
        if (!res) return;

        auto flushed = comm.flushAll();
        if (!flushed) return;

        result->ok = true;
        return;
    }

    // Receiver nodes
    char buff[REPLY_MESSAGE_SIZE]{};
    auto received = comm.recvData(1, buff, REPLY_MESSAGE_SIZE);
    if (!received) return;

    result->received.assign(buff, buff + *received);
    result->ok = true;
}

TEST(SocketMPCommunicationTest, MultiNodeBroadcastReceiversGetIdenticalMessage) {
    using Peer = std::pair<int, NodeNetworkConfig>;

    std::array<Peer, 4> nodes{
        {
            {1, NodeNetworkConfig{"127.0.0.1", 11000}},
            {2, NodeNetworkConfig{"127.0.0.1", 12000}},
            {3, NodeNetworkConfig{"127.0.0.1", 13000}},
            {4, NodeNetworkConfig{"127.0.0.1", 14000}},
        }
    };

    std::string broadcastMsg = randomMessage();
    ASSERT_EQ(broadcastMsg.size(), RANDOM_BROADCAST_MESSAGE_SIZE);

    std::array<NodeResult, 4> results;

    std::vector<std::thread> threads;
    threads.reserve(nodes.size());

    for (size_t i = 0; i < nodes.size(); ++i) {
        auto [peerId, peerCfg] = nodes[i];

        threads.emplace_back([&, i, peerId, peerCfg]() {
            runNode(peerId, peerCfg, nodes, &broadcastMsg, &results[i]);
        });
    }

    for (auto &t: threads)
        t.join();

    // Node 1 must succeed
    EXPECT_TRUE(results[0].ok) << "Node 1 failed to broadcast";

    // Nodes 2–4 must succeed and receive identical data
    for (int i = 1; i < 4; i++) {
        EXPECT_TRUE(results[i].ok) << "Node " << (i + 1) << " failed to receive";
        EXPECT_EQ(results[i].received, broadcastMsg)
            << "Node " << (i + 1) << " received incorrect message";
    }
}

#include <gtest/gtest.h>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <random>

#include <otpqcom/NetIO/SocketMPChannel.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;


constexpr int CLIENT_MESSAGE_SIZE = 50;
constexpr int REPLY_MESSAGE_SIZE = 50;

std::string randomMessage() {
    static thread_local std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist('A', 'Z');
    std::string s;
    s.reserve(CLIENT_MESSAGE_SIZE);
    for (int i = 0; i < CLIENT_MESSAGE_SIZE; ++i)
        s.push_back(static_cast<char>(dist(rng)));
    return s;
}

namespace {
    // Helper to run a node in its own thread
    void runNode(
        int selfId,
        NodeNetworkConfig selfCfg,
        std::span<std::pair<int, NodeNetworkConfig> > peers) {
        // Construct communication channel
        SocketMPChannel comm{selfId, selfCfg, peers};


        // Node 1 broadcasts
        if (selfId == 1) {
            auto randomMsg = randomMessage();

            auto res = comm.broadcastData(randomMsg.data(), randomMsg.size());

            if (!res) {
                std::cerr << "[Node 1] Broadcast failed: " << res.error() << "\n";
                return;
            }

            if (auto flushed = comm.flushAll(); !flushed) {
                std::cerr << "[Node 1] Flush failed: " << flushed.error() << "\n";
                return;
            }

            std::cout << "[Node 1] Broadcasted " << *res << " bytes (in total to all others in the group)\n";
            return;
        }

        // Other nodes receive from node 1
        char buff[REPLY_MESSAGE_SIZE]{};
        auto received = comm.recvData(1, buff, REPLY_MESSAGE_SIZE);

        if (!received) {
            std::cerr << "[Node " << selfId << "] Receive failed: "
                    << received.error() << "\n";
            return;
        }

        buff[*received] = '\0';
        std::cout << "[Node " << selfId << "] Got: " << buff << "\n";
    }
} // namespace


TEST(SocketMPCommunicationTest, MultiThreadedInProcess) {
    using Peer = std::pair<int, NodeNetworkConfig>;

    std::array<Peer, 4> nodes{
        {
            {1, {"127.0.0.1", 11000}},
            {2, {"127.0.0.1", 12000}},
            {3, {"127.0.0.1", 13000}},
            {4, {"127.0.0.1", 14000}},
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(nodes.size());

    for (auto &[peerId, peerCfg]: nodes) {
        threads.emplace_back([peerId, peerCfg, &nodes]() {
            // Directly pass ALL peers. No filtering here.
            runNode(peerId, peerCfg, nodes);
        });
    }

    for (auto &t: threads)
        t.join();

    SUCCEED();
}

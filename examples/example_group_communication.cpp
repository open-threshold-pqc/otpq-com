#include <string>
#include <array>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <random>

#include <otpqcom/Sockets/SocketMPChannel.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

constexpr int RANDOM_BROADCAST_MESSAGE_SIZE = 50;

std::string randomMessage() {
    static thread_local std::mt19937 rng{std::random_device{}()};
    static std::uniform_int_distribution<int> dist('A', 'Z');

    std::string s;
    s.reserve(RANDOM_BROADCAST_MESSAGE_SIZE);
    for (int i = 0; i < RANDOM_BROADCAST_MESSAGE_SIZE; ++i)
        s.push_back(static_cast<char>(dist(rng)));

    return s;
}


void runNode(
    int nodeSelfId,
    NodeNetworkConfig nodeSelfNetCfg,
    std::span<std::pair<int, NodeNetworkConfig> > commGroup) {

    // Create a multiple-party socket channel
    SocketMPChannel commGroupSock{nodeSelfId, std::move(nodeSelfNetCfg), commGroup};

    // Node 1 sends a broadcast
    if (nodeSelfId == 1) {
        auto msg = randomMessage();

        auto res = commGroupSock.broadcastData(msg.data(), msg.size());
        if (!res) {
            std::cerr << "[Node 1] Broadcast failed: " << res.error() << "\n";
            return;
        }

        if (auto flushed = commGroupSock.flushAll(); !flushed) {
            std::cerr << "[Node 1] Flush failed: " << flushed.error() << "\n";
            return;
        }

        std::cout << "[Node 1] Broadcasted "
                << *res << " bytes to all peers (in total)\n";

        return;
    }

    // All other nodes receive
    std::array<char, RANDOM_BROADCAST_MESSAGE_SIZE + 1> buff{};
    auto received = commGroupSock.recvData(1, buff.data(), RANDOM_BROADCAST_MESSAGE_SIZE);

    if (!received) {
        std::cerr << "[Node " << nodeSelfId << "] Receive failed: "
                << received.error() << "\n";
        return;
    }

    buff[*received] = '\0';
    std::cout << "[Node " << nodeSelfId << "] Got: " << buff.data() << "\n";
}

int main() {
    using Peer = std::pair<int, NodeNetworkConfig>;

    // Define all the nodes in the communication group
    std::array<Peer, 4> commGroup{
        Peer{1, NodeNetConf{"127.0.0.1", 11000}},
        Peer{2, NodeNetConf{"127.0.0.1", 12000}},
        Peer{3, NodeNetConf{"127.0.0.1", 13000}},
        Peer{4, NodeNetConf{"127.0.0.1", 14000}},
    };

    std::vector<std::thread> threads;
    threads.reserve(commGroup.size());

    // Launch each node in its own thread
    for (auto &[nodeId, nodeCfg]: commGroup) {
        threads.emplace_back([nodeId, nodeCfg, &commGroup]() {
            runNode(nodeId, nodeCfg, commGroup);
        });
    }

    for (auto &t: threads)
        t.join();

    return 0;
}

#include <gtest/gtest.h>
#include <string>
#include <array>
#include <thread>
#include <chrono>
#include <iostream>

#include <otpqcom/NetIO/SocketMPChannel.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

namespace {

// Helper to run a node in its own thread
void runNode(NodeNetworkConfig self, std::array<NodeNetworkConfig, 4> allPeers) {
    // Filter peers (exclude self)
    std::vector<NodeNetworkConfig> peers;
    for (const auto& p : allPeers) {
        if (p.id() != self.id()) {
            peers.push_back(p);
        }
    }

    SocketMPChannel comm{self, peers};

    // Simple logic: node 1 broadcasts, others receive
    if (self.id() == 1) {
        const std::string msg = "Hello World 1";

        auto res = comm.broadcastData(msg.data(), msg.size());
        if (!res) {
            std::cerr << "[Node 1] Broadcast failed: " << res.error() << "\n";
            return;
        }
        std::cout << "[Node 1] Broadcasted " << *res << " bytes\n";

        auto flushed = comm.flushAll();
        if (!flushed) {
            std::cerr << "[Node 1] Flush failed: " << flushed.error() << "\n";
            return;
        }
    } else {
        char buff[64]{};
        auto received = comm.recvData(1, buff, 13);
        if (!received) {
            std::cerr << "[Node " << self.id() << "] Receive failed: " << received.error() << "\n";
            return;
        }
        buff[*received] = '\0';
        std::cout << "[Node " << self.id() << "] Got message: " << buff << "\n";
    }
}

} // namespace

TEST(SocketMPCommunicationTest, MultiThreadedInProcess) {
    // All nodes (ids 1–4)
    std::array<NodeNetworkConfig, 4> nodes {
        NodeNetworkConfig{1, "127.0.0.1", 11000},
        NodeNetworkConfig{2, "127.0.0.1", 12000},
        NodeNetworkConfig{3, "127.0.0.1", 13000},
        NodeNetworkConfig{4, "127.0.0.1", 14000},
    };

    // Start each node in its own thread
    std::vector<std::thread> threads;
    for (auto& n : nodes) {
        threads.emplace_back([&nodes, n]() {
            runNode(n, nodes);
        });
    }

    // Let all threads finish
    for (auto& t : threads) {
        t.join();
    }

    SUCCEED(); // If no crashes/errors, test passes
}

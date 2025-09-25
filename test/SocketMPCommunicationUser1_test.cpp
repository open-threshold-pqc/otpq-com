#include <gtest/gtest.h>
#include <string>
#include <array>
#include <thread>
#include <chrono>

#include <otpqcom/NetIO/SocketMPCommunication.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(SocketMPCommunicationTest, SocketMPCommunication) {
    const NodeNetworkConfig cfg{1, "127.0.0.1", 11000};

    std::array<NodeNetworkConfig, 3> peers {
        NodeNetworkConfig {2, "127.0.0.1", 12000},
        NodeNetworkConfig {3, "127.0.0.1", 13000},
        NodeNetworkConfig {4, "127.0.0.1", 14000},
    };

    SocketMPCommunication mpConnection{cfg, peers};


    // Broadcast message
    auto res = mpConnection.broadcastData("Hello World 1", 13);
    ASSERT_TRUE(res.has_value()) << "Broadcast failed: " << res.error();

    // Flush all sockets
    auto flushed = mpConnection.flushAll();
    ASSERT_TRUE(flushed.has_value()) << "Flush failed: " << flushed.error();

    // Give peers time to process (dummy wait, since we’re not running actual peers in this test)
    std::this_thread::sleep_for(std::chrono::seconds{1});
}

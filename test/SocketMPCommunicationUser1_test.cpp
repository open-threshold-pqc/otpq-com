#include <gtest/gtest.h>
#include <string>

#include <otpqcom/NetIO/SocketMPCommunication.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(SocketMPCommunicationTest, SocketMPCommunication) {
    const NodeNetworkConfig cfg{1, "127.0.0.1", 11000};

    std::array<NodeNetworkConfig, 2> peers {
        NodeNetworkConfig {2, "127.0.0.1", 12000},
        NodeNetworkConfig {3, "127.0.0.1", 13000},
    };

    SocketMPCommunication mpConnection{cfg, peers};


    mpConnection.broadcastData("Hello World 1", 13);
    mpConnection.flushAll();

    sleep(5);
}

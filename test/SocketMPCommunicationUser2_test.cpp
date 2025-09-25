#include <gtest/gtest.h>
#include <string>
#include <array>
#include <iostream>

#include <otpqcom/NetIO/SocketMPCommunication.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(SocketMPCommunicationTest, SocketMPCommunication) {
    const NodeNetworkConfig cfg{2, "127.0.0.1", 12000};

    std::array<NodeNetworkConfig, 3> peers {
        NodeNetworkConfig {1, "127.0.0.1", 11000},
        NodeNetworkConfig {3, "127.0.0.1", 13000},
        NodeNetworkConfig {4, "127.0.0.1", 14000},
    };

    SocketMPCommunication mpConnection{cfg, peers};

    char buff[100]{};

    // Try receiving 13 bytes from peer 1
    auto received = mpConnection.recvData(1, buff, 13);
    ASSERT_TRUE(received.has_value())
        << "recvData failed: " << received.error();

    // Null-terminate and print
    buff[*received] = '\0';
    std::cout << "Received from peer 1: " << buff << "\n";
}

#include <gtest/gtest.h>
#include <string>
#include <array>
#include <iostream>

#include <otpqcom/NetIO/SocketMPCommunication.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(SocketMPCommunicationTest, SocketMPCommunication) {
    const NodeNetworkConfig cfg{4, "127.0.0.1", 14000};

    std::array<NodeNetworkConfig, 3> peers {
        NodeNetworkConfig {1, "127.0.0.1", 11000},
        NodeNetworkConfig {2, "127.0.0.1", 12000},
        NodeNetworkConfig {3, "127.0.0.1", 13000},
    };

    SocketMPCommunication mpConnection{cfg, peers};

    char buff[100]{};

    auto received = mpConnection.recvData(1, buff, 13);

    ASSERT_TRUE(received.has_value())
        << "recvData failed: " << received.error();

    // Null-terminate string with actual number of bytes received
    buff[*received] = '\0';

    std::cout << "Received from peer 1: " << buff << "\n";
}

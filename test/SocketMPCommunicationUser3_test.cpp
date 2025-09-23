#include <gtest/gtest.h>
#include <string>

#include <otpqcom/NetIO/SocketMPCommunication.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(SocketMPCommunicationTest, SocketMPCommunication) {
    const NodeNetworkConfig cfg{3, "127.0.0.1", 13000};

    std::array<NodeNetworkConfig, 2> peers {
        NodeNetworkConfig {1, "127.0.0.1", 11000},
        NodeNetworkConfig {2, "127.0.0.1", 12000},
    };

    SocketMPCommunication mpConnection{cfg, peers};

    char buff[100] {};

    mpConnection.recvData(1, buff, 13);

    std::cout << buff << "\n";

}

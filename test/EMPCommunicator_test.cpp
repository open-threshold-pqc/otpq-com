#include <gtest/gtest.h>
#include <otpqcom/NetIO/EMPCommunicator.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(EMPCommunicatorTest, ValidConstruction) {
    const NodeNetworkConfig cfg{1, "127.0.0.1", 9000};
    NodeNetworkConfig peer{1, "127.0.0.1", 9000};
    EMPCommunicator emp_comm{cfg};
}

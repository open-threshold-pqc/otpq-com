#include <gtest/gtest.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(NodeNetworkConfigTest, ValidConstruction) {
    const NodeNetworkConfig cfg{1, "127.0.0.1", 9000};
    EXPECT_EQ(cfg.id(), 1);
    EXPECT_EQ(cfg.ip(), "127.0.0.1");
    EXPECT_EQ(cfg.base_port(), 9000);
}

TEST(NodeNetworkConfigTest, ValidZeroPort) {
    const NodeNetworkConfig cfg{1, "127.0.0.1", 0};
    EXPECT_EQ(cfg.id(), 1);
    EXPECT_EQ(cfg.ip(), "127.0.0.1");
    EXPECT_EQ(cfg.base_port(), 0);
}

TEST(NodeNetworkConfigTest, InValidIpAddress) {
    EXPECT_THROW((NodeNetworkConfig{1, "123.0.1", 5000}), std::invalid_argument);
}

TEST(NodeNetworkConfigTest, InvalidIdThrows) {
    EXPECT_THROW((NodeNetworkConfig{0, "127.0.0.1", 9000}), std::invalid_argument);
    EXPECT_THROW((NodeNetworkConfig{-1, "127.0.0.1", 9000}), std::invalid_argument);
}

TEST(NodeNetworkConfigTest, InvalidPortThrows) {
    EXPECT_THROW((NodeNetworkConfig{1, "127.0.0.1", 80}), std::invalid_argument);
    EXPECT_THROW((NodeNetworkConfig{2, "127.0.0.1", 22}), std::invalid_argument);
}

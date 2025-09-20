#include <gtest/gtest.h>
#include <otpqcom/NodeNetworkConfig.h>

using namespace otpq::network;

TEST(PartyConfigTest, ValidConstruction) {
    const NodeNetworkConfig cfg{1, "127.0.0.1", 9000};
    EXPECT_EQ(cfg.id(), 1);
    EXPECT_EQ(cfg.ip(), "127.0.0.1");
    EXPECT_EQ(cfg.base_port(), 9000);
}

TEST(PartyConfigTest, InvalidIdThrows) {
    EXPECT_THROW((NodeNetworkConfig{0, "127.0.0.1", 9000}), std::invalid_argument);
    EXPECT_THROW((NodeNetworkConfig{-1, "127.0.0.1", 9000}), std::invalid_argument);

}

TEST(PartyConfigTest, InvalidPortThrows) {
    EXPECT_THROW((NodeNetworkConfig{1, "127.0.0.1", 80}), std::invalid_argument);
    EXPECT_THROW((NodeNetworkConfig{2, "127.0.0.1", 22}), std::invalid_argument);
}

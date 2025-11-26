#include <gtest/gtest.h>
#include <otpqcom/NodeNetworkConfig.h>

namespace {
    using otpq::network::NodeNetworkConfig;

    TEST(NodeNetworkConfig, ValidConstruction) {
        const NodeNetworkConfig cfg{"127.0.0.1", 9000};

        EXPECT_EQ(cfg.ip(), "127.0.0.1");
        EXPECT_EQ(cfg.base_port(), std::uint16_t{9000});
    }

    TEST(NodeNetworkConfig, ValidZeroPort) {
        const NodeNetworkConfig cfg{"127.0.0.1", 0};

        EXPECT_EQ(cfg.ip(), "127.0.0.1");
        EXPECT_EQ(cfg.base_port(), std::uint16_t{0});
    }

    TEST(NodeNetworkConfig, InvalidIpThrows) {
        EXPECT_THROW(NodeNetworkConfig("123.0.1", 5000), std::invalid_argument);
    }

    TEST(NodeNetworkConfig, InvalidPortThrows) {
        EXPECT_THROW(NodeNetworkConfig("127.0.0.1", 80), std::invalid_argument);
        EXPECT_THROW(NodeNetworkConfig("127.0.0.1", 22), std::invalid_argument);
    }
}

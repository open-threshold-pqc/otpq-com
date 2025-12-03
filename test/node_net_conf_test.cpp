#include <gtest/gtest.h>
#include <../src/include/otpqcom/NodeNetworkConfig.h>

namespace {
    using otpq::network::NodeNetConf;

    TEST(NodeNetworkConfig, ValidConstruction) {
        const NodeNetConf cfg{"127.0.0.1", 9000};

        EXPECT_EQ(cfg.ip(), "127.0.0.1");
        EXPECT_EQ(cfg.port(), std::uint16_t{9000});
    }

    TEST(NodeNetworkConfig, ValidZeroPort) {
        const NodeNetConf cfg{"127.0.0.1", 0};

        EXPECT_EQ(cfg.ip(), "127.0.0.1");
        EXPECT_EQ(cfg.port(), std::uint16_t{0});
    }

    TEST(NodeNetworkConfig, InvalidIpThrows) {
        EXPECT_THROW(NodeNetConf("123.0.1", 5000), std::invalid_argument);
    }

    TEST(NodeNetworkConfig, InvalidPortThrows) {

        otpq::network::NodeNetConf cfg{"127.0.0.1"};
        std::cout << cfg.ip() << std::endl;

        EXPECT_THROW(NodeNetConf("127.0.0.1", 80), std::invalid_argument);
        EXPECT_THROW(NodeNetConf("127.0.0.1", 22), std::invalid_argument);
    }

}

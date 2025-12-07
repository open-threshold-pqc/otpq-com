#include <gtest/gtest.h>

#include "otpqcom/NodeNetworkConfig.h"

using otpq::network::NodeNetworkConfig;
using otpq::network::NodeNetConf;

// -----------------------------------------------------------------------------
// Constructor tests (valid inputs)
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, ConstructWithIpAndPort) {
    NodeNetworkConfig cfg{"127.0.0.1", 8080};

    EXPECT_EQ(cfg.ip(), "127.0.0.1");
    EXPECT_EQ(cfg.port(), 8080);
}

TEST(NodeNetworkConfigTest, ConstructWithIpDefaultPort) {
    NodeNetworkConfig cfg{"192.168.1.1"};

    EXPECT_EQ(cfg.ip(), "192.168.1.1");
    EXPECT_EQ(cfg.port(), 0);
}

TEST(NodeNetworkConfigTest, ConstructWithStringView) {
    std::string ipStr = "10.0.0.5";
    std::string_view sv = ipStr;

    NodeNetworkConfig cfg{sv, 9000};

    EXPECT_EQ(cfg.ip(), "10.0.0.5");
    EXPECT_EQ(cfg.port(), 9000);
}

// -----------------------------------------------------------------------------
// Validation: invalid IP
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, InvalidIpThrows) {
    EXPECT_THROW(
        (NodeNetworkConfig{"invalid_ip", 2000}),
        std::invalid_argument
    );
}

// -----------------------------------------------------------------------------
// Validation: invalid port
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, InvalidPortBelow1024Throws) {
    EXPECT_THROW(
        (NodeNetworkConfig{"127.0.0.1", 80}),
        std::invalid_argument
    );
}

TEST(NodeNetworkConfigTest, PortZeroAllowed) {
    NodeNetworkConfig cfg{"127.0.0.1", 0};
    EXPECT_EQ(cfg.port(), 0);
}

TEST(NodeNetworkConfigTest, ValidIpAndPortAccepted) {
    NodeNetworkConfig cfg{"127.0.0.1", 2000};
    EXPECT_EQ(cfg.ip(), "127.0.0.1");
    EXPECT_EQ(cfg.port(), 2000);
}


TEST(NodeNetworkConfigTest, Port1024IsValid) {
    NodeNetworkConfig cfg{"127.0.0.1", 1024};

    EXPECT_EQ(cfg.port(), 1024);
}

TEST(NodeNetworkConfigTest, ZeroPortIsValid) {
    NodeNetworkConfig cfg{"::1", 0};

    EXPECT_EQ(cfg.ip(), "::1");
    EXPECT_EQ(cfg.port(), 0);
}

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, DefaultConstructor) {
    NodeNetworkConfig cfg;

    EXPECT_EQ(cfg.ip(), "");
    EXPECT_EQ(cfg.port(), 0);
}

// -----------------------------------------------------------------------------
// Copy constructor & copy assignment
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, CopyConstructor) {
    NodeNetworkConfig original{"::1", 7000};
    NodeNetworkConfig copy{original};

    EXPECT_EQ(copy.ip(), "::1");
    EXPECT_EQ(copy.port(), 7000);
}

TEST(NodeNetworkConfigTest, CopyAssignment) {
    NodeNetworkConfig a{"8.8.8.8", 2000};
    NodeNetworkConfig b;

    b = a;

    EXPECT_EQ(b.ip(), "8.8.8.8");
    EXPECT_EQ(b.port(), 2000);
}

// -----------------------------------------------------------------------------
// Move constructor & move assignment
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, MoveConstructor) {
    NodeNetworkConfig original{"fe80::1", 3000};
    NodeNetworkConfig moved{std::move(original)};

    EXPECT_EQ(moved.ip(), "fe80::1");
    EXPECT_EQ(moved.port(), 3000);
}

TEST(NodeNetworkConfigTest, MoveAssignment) {
    NodeNetworkConfig a{"2607:f8b0:4005:805::200e", 1234};
    NodeNetworkConfig b;

    b = std::move(a);

    EXPECT_EQ(b.ip(), "2607:f8b0:4005:805::200e");
    EXPECT_EQ(b.port(), 1234);
}

// -----------------------------------------------------------------------------
// Alias test (NodeNetConf)
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, AliasTypeWorks) {
    NodeNetConf cfg{"1.2.3.4", 4444};

    EXPECT_EQ(cfg.ip(), "1.2.3.4");
    EXPECT_EQ(cfg.port(), 4444);
}

// -----------------------------------------------------------------------------
// Additional IPv6 and noexcept checks
// -----------------------------------------------------------------------------

TEST(NodeNetworkConfigTest, IPv6AddressTest) {
    NodeNetworkConfig cfg{"2001:db8::1", 9999};

    EXPECT_EQ(cfg.ip(), "2001:db8::1");
    EXPECT_EQ(cfg.port(), 9999);
}

TEST(NodeNetworkConfigTest, NoexceptMethods) {
    const NodeNetworkConfig cfg{"::1", 4000};

    EXPECT_NO_THROW({ auto tmp = cfg.ip(); });
    EXPECT_NO_THROW({ auto p = cfg.port(); });
}

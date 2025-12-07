#include <gtest/gtest.h>
#include <otpqcom/NetHelpers.h>

using otpq::network::helpers::isValidIp;

// -----------------------------------------------------------------------------
// Valid IPv4 tests
// -----------------------------------------------------------------------------

TEST(IsValidIpTest, ValidIPv4Addresses) {
    EXPECT_TRUE(isValidIp("127.0.0.1"));
    EXPECT_TRUE(isValidIp("192.168.1.1"));
    EXPECT_TRUE(isValidIp("10.0.0.5"));
    EXPECT_TRUE(isValidIp("255.255.255.255"));
    EXPECT_TRUE(isValidIp("0.0.0.0"));
}

// -----------------------------------------------------------------------------
// Valid IPv6 tests
// -----------------------------------------------------------------------------

TEST(IsValidIpTest, ValidIPv6Addresses) {
    EXPECT_TRUE(isValidIp("::1"));
    EXPECT_TRUE(isValidIp("2001:db8::1"));
    EXPECT_TRUE(isValidIp("fe80::1234:abcd"));
    EXPECT_TRUE(isValidIp("::"));
    EXPECT_TRUE(isValidIp("2001:0db8:85a3::8a2e:0370:7334"));
}

// IPv4-mapped IPv6 is valid for inet_pton(AF_INET6)
TEST(IsValidIpTest, IPv4MappedIPv6) {
    EXPECT_TRUE(isValidIp("::ffff:192.168.1.1"));
}

// -----------------------------------------------------------------------------
// Invalid IPv4 / IPv6 formats
// -----------------------------------------------------------------------------

TEST(IsValidIpTest, InvalidFormats) {
    EXPECT_FALSE(isValidIp(""));               // empty
    EXPECT_FALSE(isValidIp(" "));              // whitespace
    EXPECT_FALSE(isValidIp("256.256.256.256"));// out of range
    EXPECT_FALSE(isValidIp("999.999.999.999"));
    EXPECT_FALSE(isValidIp("1.2.3"));          // incomplete IPv4
    EXPECT_FALSE(isValidIp("1.2.3.4.5"));      // too many octets

    EXPECT_FALSE(isValidIp("abcd"));           // not an IP
    EXPECT_FALSE(isValidIp("::::"));           // invalid IPv6
    EXPECT_FALSE(isValidIp("2001:::1"));       // malformed IPv6
    EXPECT_FALSE(isValidIp("gibberish::1"));   // invalid hex
}

// Edge cases: IPv4 with letters should fail
TEST(IsValidIpTest, InvalidCharacters) {
    EXPECT_FALSE(isValidIp("123.abc.40.20"));
    EXPECT_FALSE(isValidIp("12a.12.12.12"));
    EXPECT_FALSE(isValidIp("fe80:::zzzz"));
}

// -----------------------------------------------------------------------------
// noexcept guarantee
// -----------------------------------------------------------------------------

TEST(IsValidIpTest, NoThrow) {
    EXPECT_NO_THROW({
        bool r = isValidIp("127.0.0.1");
        (void)r;
    });
}

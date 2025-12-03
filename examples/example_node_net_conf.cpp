#include <iostream>
#include <otpqcom/NodeNetworkConfig.h>

int main() {
    using otpq::network::NodeNetConf;

    // --- Create a config with IPv4 ---
    const NodeNetConf cfg1{"127.0.0.1", 8000};
    std::cout << "cfg1.ip()   = " << cfg1.ip()   << "\n";
    std::cout << "cfg1.port() = " << cfg1.port() << "\n\n";

    // --- Create a config with IPv4 and random port to be selected by the OS ---
    const NodeNetConf cfg2{"127.0.0.1"};
    std::cout << "cfg1.ip()   = " << cfg2.ip()   << "\n";
    std::cout << "cfg1.port() = " << cfg2.port() << "\n\n";

    // --- Create a config with IPv6 ---
    const NodeNetConf cfg3{"::1", 9000};
    std::cout << "cfg2.ip()   = " << cfg3.ip()   << "\n";
    std::cout << "cfg2.port() = " << cfg3.port() << "\n\n";

    return 0;
}

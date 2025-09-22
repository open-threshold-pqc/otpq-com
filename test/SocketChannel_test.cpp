#include <gtest/gtest.h>

#include <thread>
#include <string>
#include <chrono>

#include <otpqcom/NodeNetworkConfig.h>
#include "otpqcom/NetIO/ClientSocketChannel.h"
#include "otpqcom/NetIO/ServerSocketChannel.h"

using namespace otpq::network;

TEST(SocketChannelTest, ServerClientCommunication) {
    const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};
    const NodeNetworkConfig clientCfg{2, "127.0.0.1", 10000};

    // sockets::ServerSocketChannel server{serverCfg};
    // server.awaitAndServe();

    sockets::ClientSocketChannel client{clientCfg};
    int x;
    client.sendData(&x, sizeof(x));
    // server.acceptConnection();
    // client.


}

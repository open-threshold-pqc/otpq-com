// #include <gtest/gtest.h>
//
// #include <thread>
// #include <string>
// #include <chrono>
//
// #include <otpqcom/NodeNetworkConfig.h>
// #include "otpqcom/NetIO/ClientSocketChannel.h"
// #include "otpqcom/NetIO/ServerSocketChannel.h"
//
// using namespace otpq::network;
//
// TEST(SocketChannelTest, ServerClientCommunication) {
//     const NodeNetworkConfig serverCfg{1, "127.0.0.1", 9000};
//     const NodeNetworkConfig clientCfg{2, "127.0.0.1", 10000};
//
//     // Start server thread
//     std::thread serverThread([&]() {
//         sockets::ServerSocketChannel server{serverCfg};
//         server.beginListen();
//
//         std::cout << "Received Connection\n";
//
//
//     });
//
//
//
//     // sockets::ClientSocketChannel client{clientCfg};
//     // client.nodeConnect(serverCfg);
//     // std::cout << "Client Connected\n";
//
//     while (true){};
//
//
//
// }

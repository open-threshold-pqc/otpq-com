// #include <otpqcom/NetIO/SocketChannel.h>
//
// namespace otpq::network {
//     SocketChannel::SocketChannel(NodeNetworkConfig netcfg, const NetworkNodeType nodeType)
//         : netcfg_(std::move(netcfg)), nodeType_(nodeType) {
//         if (nodeType == NetworkNodeType::SERVER)
//             setup_server();
//         else
//             setup_client();
//
//         set_no_delay();
//         //
//         // stream_ = fdopen(conn_socket_, "wb+");
//         // if (!stream_)
//         //     throw std::runtime_error("[EMPNetIOChannel]: Stream creation using fdopen() failed");
//         //
//         // buffer_.assign(otpq::network::NETWORK_BUFFER_SIZE, 0);
//         // setvbuf(stream_, buffer_.data(),
//         //         static_cast<int>(otpq::network::NetworkBufferMode::FullyBuffered),
//         //         otpq::network::NETWORK_BUFFER_SIZE);
//     }
//
//
//     SocketChannel::~SocketChannel() {
//         flush();
//         if (stream_) {
//             fclose(stream_);
//         }
//     }
//
//     void SocketChannel::nodeListen() {
//         if (nodeType_ == NetworkNodeType::SERVER) {
//             if (listen(connSocket_, 1) < 0)
//                 throw std::runtime_error(std::format("[EMPNetIOChannel]: Server nodeListen() failed ({}, {}) ",
//                                                      netcfg_.ip(),
//                                                      netcfg_.base_port()));
//
//             sockaddr_in peerSocket{};
//             socklen_t peerSocketSize = sizeof(peerSocket);
//             const int peerConnSocket = accept(connSocket_, reinterpret_cast<sockaddr *>(&peerSocket), &peerSocketSize);
//             if (peerConnSocket < 0)
//                 throw std::runtime_error(std::format("[EMPNetIOChannel]: Server accept() failed ({}, {}) ",
//                                                      netcfg_.ip(),
//                                                      netcfg_.base_port()));
//             close(connSocket_);
//             connSocket_ = peerConnSocket;
//         } else
//             throw std::runtime_error("[EMPNetIOChannel]: nodeListen() called on client");
//     }
//
//     void SocketChannel::sync() {
//         int tmp = 0;
//         if (nodeType_ == NetworkNodeType::SERVER) {
//             send_data_internal(&tmp, sizeof(tmp));
//             recv_data_internal(&tmp, sizeof(tmp));
//         } else {
//             recv_data_internal(&tmp, sizeof(tmp));
//             send_data_internal(&tmp, sizeof(tmp));
//             flush();
//         }
//     }
//
//     void SocketChannel::set_no_delay() const {
//         constexpr int one{1};
//         setsockopt(connSocket_, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
//     }
//
//     void SocketChannel::set_delay() const {
//         const int zero = 0;
//         setsockopt(connSocket_, IPPROTO_TCP, TCP_NODELAY, &zero, sizeof(zero));
//     }
//
//     void SocketChannel::flush() const {
//         fflush(stream_);
//     }
//
//     void SocketChannel::send_data_internal(const void *data, std::size_t len) {
//         std::size_t sent = 0;
//         const auto *buf = static_cast<const char *>(data);
//
//         while (sent < len) {
//             const auto res = fwrite(buf + sent, 1, len - sent, stream_);
//             if (res > 0) {
//                 sent += res;
//             } else {
//                 throw std::runtime_error("NetIO: send failed");
//             }
//         }
//         hasSent_ = true;
//     }
//
//     void SocketChannel::recv_data_internal(void *data, std::size_t len) {
//         if (hasSent_) {
//             fflush(stream_);
//         }
//         hasSent_ = false;
//
//         std::size_t received = 0;
//         auto *buf = static_cast<char *>(data);
//
//         while (received < len) {
//             const auto res = fread(buf + received, 1, len - received, stream_);
//             if (res > 0) {
//                 received += res;
//             } else {
//                 throw std::runtime_error("NetIO: recv failed");
//             }
//         }
//     }
//
//     void SocketChannel::setup_server() {
//         sockaddr_in serv{};
//         serv.sin_family = AF_INET;
//         if (inet_pton(AF_INET, netcfg_.ip().data(), &serv.sin_addr) <= 0) {
//             throw std::runtime_error(std::format("[EMPNetIOChannel]: Server Ip set failed ({}, {}) ", netcfg_.ip(),
//                                      netcfg_.base_port()));
//         }
//
//         serv.sin_port = htons(netcfg_.base_port());
//
//         connSocket_ = socket(AF_INET, SOCK_STREAM, 0);
//         if (connSocket_ < 0)
//             throw std::runtime_error(std::format("[EMPNetIOChannel]: Server socket() failed ({}, {}) ", netcfg_.ip(),
//                                                  netcfg_.base_port()));
//
//         constexpr int reuse{1};
//         setsockopt(connSocket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
//
//         if (bind(connSocket_, reinterpret_cast<sockaddr *>(&serv), sizeof(serv)) < 0)
//             throw std::runtime_error(std::format("[EMPNetIOChannel]: Server bind() failed ({}, {}) ", netcfg_.ip(),
//                                                  netcfg_.base_port()));
//
//         // if (listen(mysocket, 1) < 0) {
//         //     throw std::runtime_error("NetIO: listen() failed");
//         // }
//         //
//         // sockaddr_in dest{};
//         // socklen_t socksize = sizeof(dest);
//         // conn_socket_ = accept(mysocket, reinterpret_cast<sockaddr *>(&dest), &socksize);
//         // if (conn_socket_ < 0) {
//         //     throw std::runtime_error("NetIO: accept() failed");
//         // }
//         //
//         // close(mysocket);
//     }
//
//     void SocketChannel::setup_client() {
//         connSocket_ = socket(AF_INET, SOCK_STREAM, 0);
//         if (connSocket_ < 0)
//             throw std::runtime_error(std::format("[EMPNetIOChannel]: Client socket() failed ({}, {}) ", netcfg_.ip(),
//                                                  netcfg_.base_port()));
//
//         sockaddr_in local{};
//         local.sin_family = AF_INET;
//         local.sin_addr.s_addr = inet_addr(netcfg_.ip().data());
//         local.sin_port = htons(netcfg_.base_port());
//
//         if (bind(connSocket_, reinterpret_cast<sockaddr *>(&local), sizeof(local)) < 0) {
//             close(connSocket_);
//             throw std::runtime_error(std::format("[EMPNetIOChannel]: Client bind() failed ({}, {}) ", netcfg_.ip(),
//                                                  netcfg_.base_port()));
//         }
//
//
//         // sockaddr_in dest{};
//         // dest.sin_family = AF_INET;
//         // dest.sin_addr.s_addr = inet_addr(address.data());
//         // dest.sin_port = htons(port);
//         //
//         // while (true) {
//         //     conn_socket_ = socket(AF_INET, SOCK_STREAM, 0);
//         //     if (conn_socket_ < 0) {
//         //         throw std::runtime_error("NetIO: socket() failed (client)");
//         //     }
//         //
//         //     if (connect(conn_socket_, reinterpret_cast<sockaddr *>(&dest), sizeof(dest)) == 0) {
//         //         break; // success
//         //     }
//         //
//         //     close(conn_socket_);
//         //     usleep(1000); // retry after 1 ms
//         // }
//     }
// } // namespace emp::network

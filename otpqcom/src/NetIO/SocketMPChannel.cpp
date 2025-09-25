#include <ranges>
#include <format>
#include <utility>
#include <iostream>

#include <otpqcom/NetIO/SocketMPChannel.h>

namespace otpq::network {
    SocketMPChannel::SocketMPChannel(NodeNetworkConfig self,
                                                 std::span<NodeNetworkConfig> peers)
        : MPChannel(std::move(self), peers) {
        /* Server role: handle peers with smaller IDs */
        for (const auto &peer: peers_) {
            if (peer.id() < netcfg_.id()) {
                NodeNetworkConfig listenCfg{netcfg_.id(), netcfg_.ip(), netcfg_.base_port() + peer.id()};
                auto &serverSocket = roleServerConnections_.try_emplace(peer.id(), listenCfg).first->second;

                if (auto res = serverSocket.awaitConnection(); !res) {
                    throw std::runtime_error(std::format(
                        "[SocketMPCommunication] server awaitConnection for peer {} failed: {}",
                        peer.id(), res.error()));
                }
            }
        }

        /* Client role: handle peers with larger IDs */
        for (const auto &peer: peers_) {
            if (peer.id() > netcfg_.id()) {
                NodeNetworkConfig connectCfg{peer.id(), peer.ip(), peer.base_port() + netcfg_.id()};
                auto &clientSocket = roleClientConnections_.try_emplace(peer.id(), netcfg_).first->second;

                if (auto res = clientSocket.nodeConnect(connectCfg); !res) {
                    throw std::runtime_error(std::format(
                        "[SocketMPCommunication] client nodeConnect to peer {} failed: {}",
                        peer.id(), res.error()));
                }
            }
        }

        /* Accept all pending connections from server role */
        for (auto &peerSocket: roleServerConnections_ | std::views::values) {
            if (auto res = peerSocket.acceptConnection(); !res) {
                throw std::runtime_error(std::format(
                    "[SocketMPCommunication] server acceptConnection failed: {}",
                    res.error()));
            }
        }
    }

    SocketMPChannel::~SocketMPChannel() = default;

    std::expected<std::size_t, std::string>
    SocketMPChannel::sendData(const int peerId, const void *data, std::size_t len) {
        return withPeerSocket(peerId, [&](auto &socket) -> std::expected<std::size_t, std::string> {
            auto res = socket.sendData(data, len);
            if (!res) {
                return std::unexpected(std::format(
                    "[SocketMPCommunication] sendData to peer {} failed: {}",
                    peerId, res.error()));
            }
            return res.value();
        });
    }

    std::expected<void, std::string>
    SocketMPChannel::flush(const int peerId) {
        return withPeerSocket(peerId, [&](auto &socket) -> std::expected<void, std::string> {
            if (auto res = socket.streamFlush(); !res) {
                return std::unexpected(std::format(
                    "[SocketMPCommunication] flush for peer {} failed: {}",
                    peerId, res.error()));
            }
            return {};
        });
    }

    std::expected<std::size_t, std::string>
    SocketMPChannel::broadcastData(const void *data, const std::size_t len) {
        std::size_t totalSent = 0;

        for (auto &peerSocket: roleServerConnections_ | std::views::values) {
            if (auto res = peerSocket.sendData(data, len); !res) {
                return std::unexpected(std::format(
                    "[SocketMPCommunication] broadcastData (server role) failed: {}",
                    res.error()));
            } else {
                totalSent += *res;
            }
        }

        for (auto &peerSocket: roleClientConnections_ | std::views::values) {
            if (auto res = peerSocket.sendData(data, len); !res) {
                return std::unexpected(std::format(
                    "[SocketMPCommunication] broadcastData (client role) failed: {}",
                    res.error()));
            } else {
                totalSent += *res;
            }
        }

        return totalSent;
    }


    std::expected<void, std::string>
    SocketMPChannel::flushAll() {
        for (auto &peerSocket: roleServerConnections_ | std::views::values) {
            if (auto res = peerSocket.streamFlush(); !res) {
                return std::unexpected(std::format(
                    "[SocketMPCommunication] flushAll (server role) failed: {}",
                    res.error()));
            }
        }
        for (auto &peerSocket: roleClientConnections_ | std::views::values) {
            if (auto res = peerSocket.streamFlush(); !res) {
                return std::unexpected(std::format(
                    "[SocketMPCommunication] flushAll (client role) failed: {}",
                    res.error()));
            }
        }
        return {};
    }

    std::expected<std::size_t, std::string>
    SocketMPChannel::recvData(const int peerId, void *data, std::size_t len) {
        return withPeerSocket(peerId, [&](auto &socket) -> std::expected<std::size_t, std::string> {
            auto res = socket.recvData(data, len);
            if (!res) {
                return std::unexpected(std::format(
                    "[SocketMPCommunication] recvData from peer {} failed: {}",
                    peerId, res.error()));
            }
            return res.value();
        });
    }

    template<typename Func>
    auto SocketMPChannel::withPeerSocket(int peerId, Func &&fn)
        -> decltype(fn(std::declval<sockets::ServerSocketChannel &>())) {
        if (peerId == netcfg_.id()) {
            return std::unexpected("[SocketMPCommunication] cannot target self");
        }

        if (auto it = roleClientConnections_.find(peerId); it != roleClientConnections_.end()) {
            return fn(it->second);
        }

        if (auto it = roleServerConnections_.find(peerId); it != roleServerConnections_.end()) {
            return fn(it->second);
        }

        return std::unexpected(std::format(
            "[SocketMPCommunication] peerId {} not found in connections", peerId));
    }
} // namespace otpq::network

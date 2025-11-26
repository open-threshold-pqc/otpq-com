#include <ranges>
#include <format>
#include <utility>
#include <iostream>

#include <otpqcom/NetIO/SocketMPChannel.h>

namespace otpq::network {
    SocketMPChannel::SocketMPChannel(
            int selfId,
            NodeNetworkConfig selfCfg,
            std::span<std::pair<int, NodeNetworkConfig>> peers)
        : MPChannel(std::move(selfCfg)),
          selfId_{selfId}
    {
        /* --- Server role: peers with smaller IDs --- */
        for (auto &peerId: peers | std::views::keys) {

            if (peerId == selfId_) continue;

            if (peerId < selfId_) {

                NodeNetworkConfig listenCfg{
                    netcfg_.ip(),
                    static_cast<std::uint16_t>(netcfg_.basePort() + peerId)
                };

                auto& serverSocket =
                    roleServerConnections_.try_emplace(peerId, listenCfg).first->second;

                if (auto res = serverSocket.awaitConnection(); !res) {
                    throw std::runtime_error(std::format(
                        "server awaitConnection for peer {} failed: {}",
                        peerId, res.error()));
                }
            }
        }

        /* --- Client role: peers with larger IDs --- */
        for (auto& [peerId, peerCfg] : peers) {

            if (peerId == selfId_) continue;

            if (peerId > selfId_) {

                NodeNetworkConfig connectCfg{
                    peerCfg.ip(),
                    static_cast<std::uint16_t>(peerCfg.basePort() + selfId_)
                };


                auto& clientSocket =
                    roleClientConnections_.try_emplace(peerId, netcfg_).first->second;

                if (auto res = clientSocket.nodeConnect(connectCfg); !res) {
                    throw std::runtime_error(std::format(
                        "client nodeConnect to peer {} failed: {}",
                        peerId, res.error()));
                }

            }
        }

        /* --- Accept pending server connections --- */
        for (auto& peerSocket : roleServerConnections_ | std::views::values) {

            if (auto res = peerSocket.acceptConnection(); !res) {
                throw std::runtime_error(std::format(
                    "server acceptConnection failed: {}", res.error()));
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
}
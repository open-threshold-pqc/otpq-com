#include <ranges>
#include <format>
#include <utility>
#include <iostream>

#include <otpqcom/Sockets/SocketMPChannel.h>

namespace otpq::network {

    SocketMPChannel::SocketMPChannel(
        int selfId,
        NodeNetworkConfig selfCfg,
        std::span<std::pair<int, NodeNetworkConfig>> peers
    )
        : MPChannel(std::move(selfCfg)),
          selfId_{selfId}
    {
        using std::views::keys;
        using std::views::values;

        /* ----------------------------------------------------
         *  Server role: handle peers with smaller IDs
         * ---------------------------------------------------- */
        for (const auto &peerId : peers | keys) {

            if (peerId == selfId_) continue;

            if (peerId < selfId_) {

                // Server listens on:  self.port() + peerId
                NodeNetworkConfig listenCfg{
                    netcfg_.ip(),
                    static_cast<std::uint16_t>(netcfg_.port() + peerId)
                };

                auto &serverSocket =
                    roleServerConnections_
                        .try_emplace(peerId, listenCfg)
                        .first->second;

                if (auto res = serverSocket.listen(); !res) {
                    throw std::runtime_error(std::format(
                        "Server listen for peer {} failed: {}",
                        peerId, res.error()
                    ));
                }
            }
        }

        /* ----------------------------------------------------
         *  Client role: handle peers with larger IDs
         * ---------------------------------------------------- */
        for (auto &[peerId, peerCfg] : peers) {

            if (peerId == selfId_) continue;

            if (peerId > selfId_) {

                // Client connects to: peer.port() + selfId_
                NodeNetworkConfig connectCfg{
                    peerCfg.ip(),
                    static_cast<std::uint16_t>(peerCfg.port() + selfId_)
                };

                auto &clientSocket =
                    roleClientConnections_
                        .try_emplace(peerId, netcfg_) // local config used for bind()
                        .first->second;

                if (auto res = clientSocket.connect(connectCfg); !res) {
                    throw std::runtime_error(std::format(
                        "Client nodeConnect to peer {} failed: {}",
                        peerId, res.error()
                    ));
                }
            }
        }

        /* ----------------------------------------------------
         *  Accept pending server connections
         * ---------------------------------------------------- */
        for (auto &serverSocket : roleServerConnections_ | values) {
            if (auto res = serverSocket.accept(); !res) {
                throw std::runtime_error(std::format(
                    "Server accept failed: {}",
                    res.error()
                ));
            }
        }
    }


    SocketMPChannel::~SocketMPChannel() = default;

    std::expected<std::size_t, std::string>
    SocketMPChannel::sendData(int peerId, const void *data, std::size_t len)
    {
        return withPeerSocket(peerId, [&](auto &socket)
            -> std::expected<std::size_t, std::string>
        {
            auto res = socket.sendData(data, len);
            if (!res)
                return std::unexpected(std::format(
                    "[SocketMPCommunication] sendData to peer {} failed: {}",
                    peerId, res.error()
                ));
            return *res;
        });
    }

    std::expected<void, std::string>
    SocketMPChannel::flush(int peerId)
    {
        return withPeerSocket(peerId, [&](auto &socket)
            -> std::expected<void, std::string>
        {
            if (auto res = socket.streamFlush(); !res)
                return std::unexpected(std::format(
                    "[SocketMPCommunication] flush for peer {} failed: {}",
                    peerId, res.error()
                ));
            return {};
        });
    }

    std::expected<void, std::string>
    SocketMPChannel::flushAll()
    {
        using std::views::values;

        for (auto &socket : roleServerConnections_ | values) {
            if (auto res = socket.streamFlush(); !res)
                return std::unexpected(std::format(
                    "[SocketMPCommunication] flushAll (server) failed: {}", res.error()
                ));
        }

        for (auto &socket : roleClientConnections_ | values) {
            if (auto res = socket.streamFlush(); !res)
                return std::unexpected(std::format(
                    "[SocketMPCommunication] flushAll (client) failed: {}", res.error()
                ));
        }

        return {};
    }

    std::expected<std::size_t, std::string>
    SocketMPChannel::broadcastData(const void *data, std::size_t len)
    {
        std::size_t totalSent = 0;

        using std::views::values;

        for (auto &socket : roleServerConnections_ | values) {
            if (auto res = socket.sendData(data, len); !res)
                return std::unexpected(std::format(
                    "[SocketMPCommunication] broadcast (server) failed: {}", res.error()
                ));
            else totalSent += *res;
        }

        for (auto &socket : roleClientConnections_ | values) {
            if (auto res = socket.sendData(data, len); !res)
                return std::unexpected(std::format(
                    "[SocketMPCommunication] broadcast (client) failed: {}", res.error()
                ));
            else totalSent += *res;
        }

        return totalSent;
    }


    /* ============================================================
     *  recvData()
     * ============================================================ */
    std::expected<std::size_t, std::string>
    SocketMPChannel::recvData(int peerId, void *data, std::size_t len)
    {
        return withPeerSocket(peerId, [&](auto &socket)
            -> std::expected<std::size_t, std::string>
        {
            auto res = socket.recvData(data, len);
            if (!res)
                return std::unexpected(std::format(
                    "[SocketMPCommunication] recvData from peer {} failed: {}",
                    peerId, res.error()
                ));
            return *res;
        });
    }

} // namespace otpq::network

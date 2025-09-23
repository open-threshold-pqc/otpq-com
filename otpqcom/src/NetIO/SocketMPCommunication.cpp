#include <ranges>
#include <format>
#include <utility>

#include <otpqcom/NetIO/SocketMPCommunication.h>

namespace otpq::network {
    SocketMPCommunication::SocketMPCommunication(NodeNetworkConfig self,
                                                 std::span<NodeNetworkConfig> peers)
        : MPCommunication(std::move(self), peers) {
        /* Server role: handle peers with smaller IDs */
        for (const auto &peer: peers_) {
            if (peer.id() < netcfg_.id()) {
                NodeNetworkConfig listenCfg{netcfg_.id(), netcfg_.ip(), netcfg_.base_port() + peer.id()};
                auto &serverSocket = roleServerConnections_.try_emplace(peer.id(), listenCfg).first->second;
                serverSocket.awaitConnection();
            }
        }

        /* Client role: handle peers with larger IDs */
        for (const auto &peer: peers_) {
            if (peer.id() > netcfg_.id()) {
                NodeNetworkConfig connectCfg{peer.id(), peer.ip(), peer.base_port() + netcfg_.id()};
                auto &clientSocket = roleClientConnections_.try_emplace(peer.id(), netcfg_).first->second;
                clientSocket.nodeConnect(connectCfg);
            }
        }

        /* Accept all pending connections from server role */
        for (auto &peerSocket: roleServerConnections_ | std::views::values) {
            peerSocket.acceptConnection();
        }
    }

    SocketMPCommunication::~SocketMPCommunication() = default;

    void SocketMPCommunication::sendData(const int peerId, const void *data, std::size_t len) {
        withPeerSocket(peerId, [&](auto& socket) {
            socket.sendData(data, len);
        });
    }

    void SocketMPCommunication::flush(const int peerId) {
        withPeerSocket(peerId, [&](auto& socket) {
            socket.streamFlush();
        });
    }

    void SocketMPCommunication::broadcastData(const void *data, const std::size_t len) {
        for (auto &peerSocket: roleServerConnections_ | std::views::values) {
            peerSocket.sendData(data, len);
        }
        for (auto &peerSocket: roleClientConnections_ | std::views::values) {
            peerSocket.sendData(data, len);
        }
    }

    void SocketMPCommunication::flushAll() {
        for (auto &peerSocket : roleServerConnections_ | std::views::values) {
            peerSocket.streamFlush();
        }
        for (auto &peerSocket : roleClientConnections_ | std::views::values) {
            peerSocket.streamFlush();
        }
    }


    void SocketMPCommunication::recvData(const int peerId, void *data, std::size_t len) {
        withPeerSocket(peerId, [&](auto& socket) {
            socket.recvData(data, len);
        });
    }

    template <typename Func>
    void SocketMPCommunication::withPeerSocket(int peerId, Func&& fn) {
        if (peerId == netcfg_.id()) {
            throw std::invalid_argument("[SocketMPCommunication] cannot target self");
        }

        if (auto it = roleClientConnections_.find(peerId); it != roleClientConnections_.end()) {
            fn(it->second);
            return;
        }

        if (auto it = roleServerConnections_.find(peerId); it != roleServerConnections_.end()) {
            fn(it->second);
            return;
        }

        throw std::runtime_error(
            std::format("[SocketMPCommunication] peerId {} not found in connections", peerId)
        );
    }
} // namespace otpq::network

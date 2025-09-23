#pragma once

#include <otpqcom/NetIO/MPCommunication.h>
#include <otpqcom/NetIO/ServerSocketChannel.h>
#include <otpqcom/NetIO/ClientSocketChannel.h>
#include <otpqcom/NodeNetworkConfig.h>

#include <unordered_map>

namespace otpq::network {
    /**
     * @class SocketMPCommunication
     * @brief A socket-based implementation of multi-peer communication.
     *
     * This class extends the abstract `MPCommunication` interface to provide
     * concrete communication between peers using TCP sockets.
     *
     * Responsibilities:
     * - Acts as a **server** for peers with smaller IDs.
     * - Acts as a **client** for peers with larger IDs.
     * - Manages both server and client socket channels.
     * - Provides message send, broadcast, and receive operations.
     */
    class SocketMPCommunication final : public MPCommunication {
    public:
        /**
         * @brief Construct a socket-based communication context.
         *
         * @param self  The local node’s configuration.
         * @param peers The list of peer configurations.
         *
         * The constructor sets up server/client roles depending on ID ordering:
         * - For peers with IDs smaller than `self.id()`, this node becomes a server.
         * - For peers with IDs greater than `self.id()`, this node becomes a client.
         */
        explicit SocketMPCommunication(NodeNetworkConfig self, std::span<NodeNetworkConfig> peers);

        /// @brief Destructor that closes all open socket connections.
        ~SocketMPCommunication() override;

        /**
         * @brief Send data to a specific peer.
         *
         * @param peerId Target peer ID.
         * @param data   Pointer to the data buffer.
         * @param len    Length of the buffer in bytes.
         *
         * @throws std::runtime_error if the peer ID is not found or sending fails.
         */
        void sendData(int peerId, const void *data, std::size_t len) override;

        void flush(int peerId);

        void flushAll();

        /**
         * @brief Broadcast data to all connected peers.
         *
         * @param data Pointer to the data buffer.
         * @param len  Length of the buffer in bytes.
         *
         * @throws std::runtime_error if sending fails to any peer.
         */
        void broadcastData(const void *data, std::size_t len) override;

        /**
         * @brief Receive data from a specific peer.
         *
         * @param peerId Source peer ID.
         * @param data   Pointer to the buffer where data will be stored.
         * @param len    Maximum number of bytes to read.
         *
         * @throws std::runtime_error if the peer ID is not found or receiving fails.
         */
        void recvData(int peerId, void *data, std::size_t len) override;

    private:
        /**
         * @brief Apply a function to the socket associated with a peer ID.
         *
         * Looks up the peer ID in client and server connection maps and calls
         * the provided function with the socket reference.
         *
         * @tparam Func Callable type taking `socket&` as parameter.
         * @param peerId The peer identifier.
         * @param fn The callable to apply on the socket.
         *
         * @throw std::invalid_argument if peerId refers to self.
         * @throw std::runtime_error if the peer is not found.
         */
        template <typename Func>
        void withPeerSocket(int peerId, Func&& fn);

        /// @brief Active server-side connections (peers with smaller IDs).
        std::unordered_map<int, sockets::ServerSocketChannel> roleServerConnections_;

        /// @brief Active client-side connections (peers with larger IDs).
        std::unordered_map<int, sockets::ClientSocketChannel> roleClientConnections_;
    };
} // namespace otpq::network

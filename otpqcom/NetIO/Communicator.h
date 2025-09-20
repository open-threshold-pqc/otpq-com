#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network {

    /**
     * \brief Abstract interface for node-to-node communication.
     *
     * This interface defines the contract for communication between
     * nodes in the network. Implementations may use different backends
     * (e.g., ZeroMQ, EMP, raw sockets) but must provide a consistent
     * interface for sending and receiving messages.
     *
     * Typical usage:
     * \code
     * std::unique_ptr<otpq::network::Communicator> comm =
     *     make_communicator(Backend::ZeroMQ, localConfig);
     *
     * comm->send(remoteConfig, data.data(), data.size());
     * auto reply = comm->recv(remoteConfig);
     * \endcode
     */
    class Communicator {
    public:
        /**
         * \brief Virtual destructor to ensure proper cleanup.
         *
         * Ensures derived class destructors are called correctly
         * when deleting via a base pointer.
         */
        virtual ~Communicator() = default;

        /**
         * \brief Send a message to a peer.
         *
         * \param peer Configuration of the peer node (e.g., ID, IP, port).
         * \param data Pointer to the buffer to send.
         * \param size Number of bytes to send from the buffer.
         *
         * \throws std::runtime_error Implementations may throw if
         *         the send operation fails.
         */
        virtual void send(const NodeNetworkConfig& peer,
                          const void* data,
                          std::size_t size) = 0;

        /**
         * \brief Receive a message from a peer.
         *
         * This function is expected to block until a message is received,
         * unless the implementation supports non-blocking or timeout modes.
         *
         * \param peer Configuration of the peer node.
         * \return A vector of bytes containing the received message.
         *
         * \throws std::runtime_error Implementations may throw if
         *         the receive operation fails.
         */
        virtual std::vector<std::byte> recv(const NodeNetworkConfig& peer) = 0;
    };

} // namespace otpq::network

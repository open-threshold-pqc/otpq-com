#pragma once

#include <otpqcom/NetIO/Communicator.h>
#include <otpqcom/NodeNetworkConfig.h>

namespace otpq::network {
    /**
     * \brief EMP-based implementation of the Communicator interface.
     *
     * Wraps an emp::NetIO channel to provide send/receive methods.
     */
    class EMPCommunicator final : public Communicator {
    public:
        /// Construct an EMP communicator for the given node configuration.
        explicit EMPCommunicator(NodeNetworkConfig self, std::span<NodeNetworkConfig> peers);

        /// Destroy communicator and clean up EMP resources.
        ~EMPCommunicator() override;

        /// Send data to a peer node.
        void send(const NodeNetworkConfig &peer,
                  const void *data,
                  std::size_t size) override;

        /// Receive data from a peer node.
        std::vector<std::byte> recv(const NodeNetworkConfig &peer) override;

    private:
        NodeNetworkConfig m_self; ///< This node's configuration.
        std::vector<NodeNetworkConfig> m_peers; //
    };
} // namespace otpq::network

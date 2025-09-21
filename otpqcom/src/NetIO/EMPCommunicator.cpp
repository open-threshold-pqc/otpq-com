#include <otpqcom/NetIO/EMPCommunicator.h>
#include <stdexcept>
#include <cstring>
#include <utility>

namespace otpq::network {

    EMPCommunicator::EMPCommunicator(NodeNetworkConfig self, std::span<NodeNetworkConfig> peers)
        : m_self(std::move(self)), m_peers(peers.begin(), peers.end()) {
        // TODO: Initialize emp::NetIO here (server/client setup).
        // Example:
        // m_io = std::make_unique<emp::NetIO>(config.ip().c_str(), config.base_port());
    }

    EMPCommunicator::~EMPCommunicator() {
        // TODO: cleanup if needed (emp::NetIO usually cleans up itself).
    }

    void EMPCommunicator::send(const NodeNetworkConfig &peer,
                               const void *data,
                               std::size_t size) {
        // TODO: replace with emp::NetIO::send_data
        // Example: m_io->send_data(data, size);
        (void)peer;  // avoid unused param warning for now
        (void)data;
        (void)size;
        throw std::logic_error("EMPCommunicator::send not yet implemented");
    }

    std::vector<std::byte> EMPCommunicator::recv(const NodeNetworkConfig &peer) {
        // TODO: replace with emp::NetIO::recv_data
        (void)peer;  // avoid unused param warning
        throw std::logic_error("EMPCommunicator::recv not yet implemented");
    }

} // namespace otpq::network

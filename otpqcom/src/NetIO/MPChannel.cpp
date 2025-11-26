#include <otpqcom/NetIO/MPChannel.h>
#include <algorithm>
#include <stdexcept>
#include <format>
#include <ranges>
#include <unordered_set>

namespace otpq::network {
    MPChannel::MPChannel(NodeNetworkConfig cfg)
        : netcfg_(std::move(cfg)) {
        // todo Ensure peers have unique IDs
        // std::unordered_set<int> seenIds;
        // for (const auto& peer : peers_) {
        //     if (!seenIds.insert(peer.id()).second) {
        //         throw std::runtime_error(
        //             std::format("[MPCommunication] Duplicate peer ID detected: {}", peer.id())
        //         );
        //     }
        // }
    }
} // namespace otpq::network

#pragma once

#include "app/algorithms/algorithm_impl.h"
#include "edge_orientation/ds/flip_graph.h"
#include "edge_orientation/tools/bfs.h"

namespace rpo::app::algorithms {

template <class StandardFlipGraph>
class BfsBased : public AlgorithmImpl<StandardFlipGraph> {
 public:
  static constexpr absl::string_view AlgorithmName = "bfs_based";

 protected:
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Load(const RunConfig& run_config,
                                                          const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("Load should not be called directly");
  }
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<StandardFlipGraph> orientation,
      bool& exact) override {
    exact = true;
    rpo::edge_orientation::bfs::bfs_based_orientation(*orientation);
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms
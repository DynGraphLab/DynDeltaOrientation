#pragma once

#include "app/algorithms/algorithm_impl.h"
#include "edge_orientation/ds/flip_graph.h"
#include "edge_orientation/tools/greedy.h"

namespace rpo::app::algorithms {
namespace {
using edge_orientation::ds::StandardFlipGraph;
}  // namespace

template <class StandardFlipGraph>
class ResortImpl : public AlgorithmImpl<StandardFlipGraph> {
 public:
  static constexpr absl::string_view AlgorithmName = "resort";

 protected:
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Load(const RunConfig& run_config,
                                                          const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("Load should not be called directly");
  }
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<StandardFlipGraph> orientation,
      bool& exact) override {
    exact = true;
    orientation->resort_fast();
    // edge_orientation::greedy::solve_up_to_length_one(*orientation);
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms
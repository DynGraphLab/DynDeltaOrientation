#pragma once

#include "app/algorithms/algorithm_impl.h"
#include "edge_orientation/ds/flip_graph.h"
#include "edge_orientation/tools/combined.h"

namespace rpo::app::algorithms {

template <class StandardFlipGraph>
class Combined : public AlgorithmImpl<StandardFlipGraph> {
 public:
  static constexpr absl::string_view AlgorithmName = "combined";

 protected:
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Load(const RunConfig& run_config,
                                                          const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("Load should not be called directly");
  }
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<StandardFlipGraph> orientation,
      bool& exact) override {
    exact = true;
    rpo::edge_orientation::combined::solve_by_combine(*orientation,
                                                            3 * sqrt(orientation->edge_count()));
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
template <class StandardFlipGraph>
class DfsMultipleCombined : public AlgorithmImpl<StandardFlipGraph> {
 public:
  static constexpr absl::string_view AlgorithmName = "dfs_multiple_combined";

 protected:
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Load(const RunConfig& run_config,
                                                          const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("Load should not be called directly");
  }
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<StandardFlipGraph> orientation, bool& exact,
      app::app_io::DebugInformation& mut) override {
    exact = true;
    auto bool_params = config.bool_params();
    auto int64_params = config.int64_params();

    auto count = int64_params["static"];
    if (count == 0) {
      count = sqrt((orientation->computeOutflows()) -
                   (orientation->edge_count() / (double)orientation->vertex_count()));
    }
    mut.mutable_int64_info()->insert({"eager_count", count});
    auto eager_size = int64_params["eager_size"];
    if (eager_size == 0) {
      eager_size = 100;
    }
    // std::cout << "C:" << count << std::endl;
    rpo::edge_orientation::combined::solve_by_dfs_combined_multiple(
        *orientation, count, !bool_params["leave_rest"], eager_size);
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms
#pragma once

#include "app/algorithms/algorithm_impl.h"
#include "edge_orientation/ds/flip_graph.h"
#include "edge_orientation/ds/flip_graph_seperated.h"
#include "edge_orientation/tools/dfs.h"
#include "edge_orientation/tools/dfs_naive.h"

namespace rpo::app::algorithms::edge_orientation {
namespace {
using rpo::edge_orientation::ds::SmallerGraph;
using rpo::edge_orientation::ds::StandardFlipGraph;
using rpo::edge_orientation::ds::StandardSepFlipGraph;

using StandardSepFlipGraphInt = rpo::edge_orientation::ds::SepFlipGraph<long, long>;
}  // namespace

template <class FlipGraphImpl>
class DfsBased : public AlgorithmImpl<FlipGraphImpl> {
 public:
  static constexpr absl::string_view AlgorithmName = "dfs_based";

 protected:
  absl::StatusOr<std::unique_ptr<FlipGraphImpl>> Load(const RunConfig& run_config,
                                                      const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("Load should not be called directly");
  }
  absl::StatusOr<std::unique_ptr<FlipGraphImpl>> Execute(const AlgorithmConfig& config,
                                                         std::unique_ptr<FlipGraphImpl> orientation,
                                                         bool& exact) override {
    exact = true;
    rpo::edge_orientation::dfs::solve_by_dfs(*orientation);
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
template <class FlipGraphImpl>
class DfsNaive : public AlgorithmImpl<FlipGraphImpl> {
 public:
  static constexpr absl::string_view AlgorithmName = "dfs_naive";

 protected:
  absl::StatusOr<std::unique_ptr<FlipGraphImpl>> Load(const RunConfig& run_config,
                                                      const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("Load should not be called directly");
  }
  absl::StatusOr<std::unique_ptr<FlipGraphImpl>> Execute(const AlgorithmConfig& config,
                                                         std::unique_ptr<FlipGraphImpl> orientation,
                                                         bool& exact) override {
    exact = true;
    auto bool_params = config.bool_params();
    rpo::edge_orientation::dfs_naive::solve_by_dfs(*orientation,
                                                         bool_params["reset_every_time"]);
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};

class SFlipGraphDFSImpl : public DfsBased<StandardFlipGraph> {};
class SepSFlipGraphDFSImpl : public DfsBased<StandardSepFlipGraph> {};

}  // namespace rpo::app::algorithms::edge_orientation
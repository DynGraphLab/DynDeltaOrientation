#pragma once

#include "app/algorithms/algorithm_impl.h"
#include "ds/empty_problem.h"
#include "edge_orientation/ds/flip_graph.h"
#include "edge_orientation/ds/flip_graph_seperated.h"
#include "edge_orientation/ds/smaller_graph.h"

namespace rpo::app::algorithms::edge_orientation {
namespace {
using rpo::edge_orientation::ds::StandardFlipGraph;
using rpo::edge_orientation::ds::StandardSepFlipGraph;
using rpo::edge_orientation::ds::StandardSmallerGraph;
using SimpleSetGraphProblem = rpo::ds::EmptyProblem<ds::StandardSimpleSetGraph>;
}  // namespace
template <class StandardFlipGraph>
class EmptyToFlipImpl : public AlgorithmImpl<SimpleSetGraphProblem, StandardFlipGraph> {
 protected:
  absl::StatusOr<std::unique_ptr<SimpleSetGraphProblem>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
    TIMED_FUNC(timer);
    if (hypergraph.format() != "graph" && hypergraph.format() != "seq" &&
        hypergraph.format() != "binary_uu" && hypergraph.format() != "mtx" ) {
      return absl::UnimplementedError(" Only binary_uu, graph,  mtx and seq is supported");
    }
    auto intermediate =
        rpo::app::algorithms::loadFile<ds::StandardSimpleSetGraph>(hypergraph, true);
    if (!intermediate.ok()) {
      return intermediate.status();
    }
    return std::make_unique<SimpleSetGraphProblem>(std::move(intermediate.value()));
  }
  absl::StatusOr<std::unique_ptr<StandardFlipGraph>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<SimpleSetGraphProblem> orientation,
      bool& exact) override {
    TIMED_FUNC(timer);
    exact = true;
    return std::make_unique<StandardFlipGraph>(orientation->instance());
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
class EmptyToFlip : public EmptyToFlipImpl<StandardFlipGraph> {
 public:
  static constexpr absl::string_view AlgorithmName = "empty_to_flip";
};
class EmptyToFlipSep : public EmptyToFlipImpl<StandardSepFlipGraph> {
 public:
  static constexpr absl::string_view AlgorithmName = "empty_to_flip_sep";
};
class EmptyToSmaller : public EmptyToFlipImpl<StandardSmallerGraph> {
 public:
  static constexpr absl::string_view AlgorithmName = "empty_to_smaller";
};
}  // namespace rpo::app::algorithms::edge_orientation
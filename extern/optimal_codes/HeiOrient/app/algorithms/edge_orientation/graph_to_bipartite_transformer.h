#pragma once
#include <algorithm>
#include <functional>
#include <numeric>

#include "app/algorithms/algorithm_impl.h"
#include "ds/empty_problem.h"
#include "utils/range.h"

namespace rpo::edge_orientation::binary_flow {
namespace {

using rpo::app::app_io::AlgorithmConfig;
using rpo::app::app_io::AlgorithmRunInformation;
using rpo::app::app_io::Hypergraph;
using rpo::app::app_io::Result;
using rpo::app::app_io::RunConfig;
using rpo::ds::EmptyProblem;
using rpo::ds::StandardSimpleSetGraph;
}  // namespace

template <typename BinaryFlowImpl>
class EmptyProblemToBinaryFlow
    : public ::rpo::app::algorithms::AlgorithmImpl<EmptyProblem<StandardSimpleSetGraph>,
                                                         BinaryFlowImpl> {
 public:
  // static constexpr absl::string_view AlgorithmName = "graph_to_bipartite_avx";

 protected:
  absl::StatusOr<std::unique_ptr<EmptyProblem<StandardSimpleSetGraph>>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
    if (hypergraph.format() != "graph" && hypergraph.format() != "seq" &&
        hypergraph.format() != "binary_uu" && hypergraph.format() != "mtx" &&
        hypergraph.format() != "kagen") {
      return absl::UnimplementedError(" Only binary_uu, graph, kagen, mtx and seq is supported");
    }
    auto intermediate =
        rpo::app::algorithms::loadFile<StandardSimpleSetGraph>(hypergraph, true);
    if (!intermediate.ok()) {
      return intermediate.status();
    }
    return std::make_unique<EmptyProblem<StandardSimpleSetGraph>>(std::move(intermediate.value()));
  }
  absl::StatusOr<std::unique_ptr<BinaryFlowImpl>> Execute(
      const AlgorithmConfig& config,
      std::unique_ptr<EmptyProblem<StandardSimpleSetGraph>> empty_problem, bool& exact) override {
    exact = true;
    auto orientation = std::make_unique<BinaryFlowImpl>(empty_problem->instance().edges_,
                                                        empty_problem->instance().nodes_);
    size_t u = 0;
    for (auto& vs : empty_problem->instance().entries) {
      orientation->addEdges(u++, vs.begin(), vs.end());
    }
    orientation->finish();
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::edge_orientation::binary_flow
#pragma once

#include "app/algorithms/algorithm_impl.h"
#include "ds/empty_problem.h"
#include "edge_orientation/tools/two_approx.h"

namespace rpo::app::algorithms {
namespace {
using SimpleSetGraphProblem = rpo::ds::EmptyProblem<ds::StandardSimpleSetGraph>;

}  // namespace

class TwoApprox : public AlgorithmImpl<SimpleSetGraphProblem> {
 public:
  static constexpr absl::string_view AlgorithmName = "two_approx";

 protected:
  absl::StatusOr<std::unique_ptr<SimpleSetGraphProblem>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
    if (hypergraph.format() != "graph" && hypergraph.format() != "seq" &&
        hypergraph.format() != "binary_uu" && hypergraph.format() != "mtx" ) {
      return absl::UnimplementedError(" Only binary_uu, graph, mtx and seq is supported");
    }
    auto intermediate =
        rpo::app::algorithms::loadFile<ds::StandardSimpleSetGraph>(hypergraph, true);
    if (!intermediate.ok()) {
      return intermediate.status();
    }
    return std::make_unique<SimpleSetGraphProblem>(std::move(intermediate.value()));
  }
  absl::StatusOr<std::unique_ptr<SimpleSetGraphProblem>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<SimpleSetGraphProblem> orientation,
      bool& exact) override {
    exact = true;
    auto two_ =
        rpo::edge_orientation::two_approx::compute_two_approx(orientation->instance());
    auto res = rpo::edge_orientation::two_approx::remove_up_to(orientation->instance(),
                                                                     std::floor(two_ / 2.0));
    auto res2 = std::make_unique<SimpleSetGraphProblem>(std::move(res));
    res2->setSize(two_);
    res2->setWeight(two_);

    return res2;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
class TwoApproxIF : public AlgorithmImpl<SimpleSetGraphProblem> {
 public:
  static constexpr absl::string_view AlgorithmName = "two_approx_if";

 protected:
  absl::StatusOr<std::unique_ptr<SimpleSetGraphProblem>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
    if (hypergraph.format() != "graph" && hypergraph.format() != "seq" &&
        hypergraph.format() != "binary_uu" && hypergraph.format() != "mtx") {
      return absl::UnimplementedError(" Only binary_uu, graph, mtx and seq is supported");
    }
    auto intermediate =
        rpo::app::algorithms::loadFile<ds::StandardSimpleSetGraph>(hypergraph, true);
    if (!intermediate.ok()) {
      return intermediate.status();
    }
    return std::make_unique<SimpleSetGraphProblem>(std::move(intermediate.value()));
  }
  absl::StatusOr<std::unique_ptr<SimpleSetGraphProblem>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<SimpleSetGraphProblem> orientation,
      bool& exact) override {
    exact = true;
    auto double_params = config.double_params();
    if (((double)orientation->instance().edges_) / ((double)orientation->instance().nodes_) <=
        double_params["max_density"]) {
      return orientation;
    }
    auto two_ =
        rpo::edge_orientation::two_approx::compute_two_approx(orientation->instance());
    auto res = rpo::edge_orientation::two_approx::remove_up_to(orientation->instance(),
                                                                     std::floor(two_ / 2.0));
    auto res2 = std::make_unique<SimpleSetGraphProblem>(std::move(res));
    res2->setSize(two_);
    res2->setWeight(two_);

    return res2;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms
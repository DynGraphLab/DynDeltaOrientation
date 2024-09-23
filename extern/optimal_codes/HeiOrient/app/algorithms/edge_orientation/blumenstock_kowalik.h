#pragma once
#include <functional>

#include "app/algorithms/edge_orientation/algorithm_impl.h"
#include "app/algorithms/edge_orientation/graph_to_bipartite_transformer.h"
#include "edge_orientation/blumenstock/kowalik.h"
#include "edge_orientation/ds/flow_graph.h"
#include "edge_orientation/flows/dinic.h"
#include "edge_orientation/flows/push_relabel.h"

namespace rpo::app::algorithms {
namespace {
using BlumenstockPushRelabel = rpo::edge_orientation::blumenstock::BlumenstockPushRelabelDS<
    rpo::edge_orientation::ds::FlowGraph<unsigned int, size_t, long>>;
}  // namespace

struct EmptyProblemToBlumenstockPushRelabel
    : public ::rpo::edge_orientation::binary_flow::EmptyProblemToBinaryFlow<
          BlumenstockPushRelabel> {
  static constexpr absl::string_view AlgorithmName = "empty_to_blumenstock";
};

class BlumenstockPR : public AlgorithmImpl<BlumenstockPushRelabel> {
 public:
  static constexpr absl::string_view AlgorithmName = "flow";

 protected:
  absl::StatusOr<std::unique_ptr<BlumenstockPushRelabel>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("This function should not be used.");
  }
  absl::StatusOr<std::unique_ptr<BlumenstockPushRelabel>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<BlumenstockPushRelabel> orientation,
      bool& exact) override {
    exact = true;
    auto str_params = config.string_params();
    orientation->solve_repeat();

    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};

class BlumenstockGP : public AlgorithmImpl<BlumenstockPushRelabel> {
 public:
  static constexpr absl::string_view AlgorithmName = "gp";

 protected:
  absl::StatusOr<std::unique_ptr<BlumenstockPushRelabel>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
    return absl::UnimplementedError("This function should not be used.");
  }
  absl::StatusOr<std::unique_ptr<BlumenstockPushRelabel>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<BlumenstockPushRelabel> orientation,
      bool& exact) override {
    exact = true;
    auto bool_params = config.bool_params();
    auto str_params = config.string_params();
    if (str_params["flow_algorithm"] == "push_relabel") {
      orientation->solve_pseudo<rpo::edge_orientation::flows::push_relabel<
          rpo::edge_orientation::ds::FlowGraph<unsigned int, size_t, long>>>(
          bool_params["produce_orientation"]);
    } else {
      orientation->solve_pseudo(bool_params["produce_orientation"]);
    }

    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms

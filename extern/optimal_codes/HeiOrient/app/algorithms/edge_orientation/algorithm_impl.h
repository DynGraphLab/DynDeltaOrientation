#pragma once

#include "absl/status/statusor.h"
#include "app/algorithms/algorithm_impl.h"
#include "ds/edge_orientation.h"
#include "ds/undirected_graph.h"

namespace rpo {
namespace app {
namespace algorithms {
namespace {

using rpo::ds::UndirectedGraph;

}  // namespace
using StandardEdgeOrientationProblem = rpo::ds::EdgeOrientation<UndirectedGraph<size_t>>;
class EdgeOrientationAlgorithmImpl : public AlgorithmImpl<StandardEdgeOrientationProblem> {
 public:
  absl::StatusOr<std::unique_ptr<StandardEdgeOrientationProblem>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
    auto hgr = loadFile<UndirectedGraph<size_t>>(hypergraph);
    if (!hgr.ok()) {
      return hgr.status();
    }
    auto gr = std::move(hgr.value());
    return std::make_unique<StandardEdgeOrientationProblem>(std::move(gr));
  }
};

}  // namespace algorithms
}  // namespace app
}  // namespace rpo
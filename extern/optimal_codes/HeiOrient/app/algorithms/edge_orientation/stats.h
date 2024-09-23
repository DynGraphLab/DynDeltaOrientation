#pragma once
#include <queue>

#include "app/algorithms/algorithm_impl.h"
#include "ds/empty_problem.h"

namespace rpo::app::algorithms::edge_orientation {
namespace {
using SimpleSetGraphProblem = rpo::ds::EmptyProblem<ds::StandardSimpleSetGraph>;

}  // namespace

class Stats : public AlgorithmImpl<SimpleSetGraphProblem> {
 public:
  static constexpr absl::string_view AlgorithmName = "stats";

 protected:
  absl::StatusOr<std::unique_ptr<SimpleSetGraphProblem>> Load(
      const RunConfig& run_config, const Hypergraph& hypergraph) override {
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
  absl::StatusOr<std::unique_ptr<SimpleSetGraphProblem>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<SimpleSetGraphProblem> orientation,
      bool& exact) override {
    auto str_params = config.string_params();
    if (str_params["name"] == "n") {
      orientation->setSize(orientation->instance().entries.size());
      orientation->setWeight(orientation->instance().entries.size());
    }
    if (str_params["name"] == "m") {
      orientation->setSize(orientation->instance().edges_);
      orientation->setWeight(orientation->instance().edges_);
    }
    if (str_params["name"] == "min") {
      size_t min_value = orientation->instance().nodes_;
      for (auto& val : orientation->instance().entries) {
        min_value = std::min(val.size(), min_value);
      }
      orientation->setSize(min_value);
      orientation->setWeight(min_value);
    }
    if (str_params["name"] == "max") {
      size_t max_value = 0;
      for (auto& val : orientation->instance().entries) {
        max_value = std::max(val.size(), max_value);
      }
      orientation->setSize(max_value);
      orientation->setWeight(max_value);
    }
    if (str_params["name"] == "connected_components") {
      size_t res = 0;
      std::vector<bool> visited(orientation->instance().vertex_count(), false);

      for (typename SimpleSetGraphProblem::NodeType node = 0;
           node < orientation->instance().vertex_count(); node++) {
        if (!visited[node]) {
          res++;
          std::queue<typename SimpleSetGraphProblem::NodeType> queue;
          queue.push(node);
          visited[node] = true;
          while (!queue.empty()) {
            auto elem = queue.front();
            queue.pop();
            for (auto neigb : orientation->instance().entries[elem]) {
              if (!visited[neigb]) {
                visited[neigb] = true;
                queue.push(neigb);
              }
            }
          }
        }
      }
      orientation->setSize(res);
      orientation->setWeight(res);
    }

    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms::edge_orientation
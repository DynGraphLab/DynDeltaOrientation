#pragma once
#include <functional>

#include "app/algorithms/edge_orientation/algorithm_impl.h"

namespace rpo::app::algorithms {
namespace {}  // namespace
class Naive : public EdgeOrientationAlgorithmImpl {
 public:
  static constexpr absl::string_view AlgorithmName = "naive";

 protected:
  absl::StatusOr<std::unique_ptr<StandardEdgeOrientationProblem>> Execute(
      const AlgorithmConfig& config,
      std::unique_ptr<StandardEdgeOrientationProblem> orientation) override {
    auto& instance = orientation->instance();
    for (auto e : instance.edges()) {
      orientation->SetOrientation(e, orientation->outGoingOrientationAt(instance.edge(e)[0]) <
                                         orientation->outGoingOrientationAt(instance.edge(e)[1]));
    }
    return orientation;
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms
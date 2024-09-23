

#pragma once
#include <functional>

#include "app/algorithms/edge_orientation/algorithm_impl.h"
#include "edge_orientation/ilp/ilp.h"
namespace rpo::app::algorithms {
namespace {}  // namespace
class ILP : public EdgeOrientationAlgorithmImpl {
 public:
  static constexpr absl::string_view AlgorithmName = "ilp";

 protected:
  absl::StatusOr<std::unique_ptr<StandardEdgeOrientationProblem>> Execute(
      const AlgorithmConfig& config, std::unique_ptr<StandardEdgeOrientationProblem> orientation,
      bool& exact) override {
    auto double_params = config.double_params();
    auto string_params = config.string_params();
    {
      auto status = rpo::edge_orientation::ilp::solve_ilp(*orientation.get(), exact,
                                                                double_params["timeout"]);
      if (!status.ok()) {
        return status;
      }
      return orientation;
    }
  }
  absl::Status ValidateConfig(const AlgorithmConfig& config) override { return absl::OkStatus(); }
};
}  // namespace rpo::app::algorithms
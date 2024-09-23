#pragma once

#include <vector>

#include "absl/status/status.h"

#ifdef USE_GUROBI_ENABLED
#include "gurobi_c++.h"
#else
typedef int GRBEnv;
typedef int GRBVar;
typedef int GRBModel;
#endif
namespace rpo {
namespace edge_orientation {
namespace ilp {
template <class EdgeOrientation>
absl::Status solve_ilp(EdgeOrientation& edge_orientation, bool& isExact, double time_limit) {
#ifdef USE_GUROBI_ENABLED
  std::cout << "Solving using ILP" << std::endl;
  try {
    auto& G = edge_orientation.instance();
    auto env = std::make_shared<GRBEnv>();
    GRBModel model = GRBModel(*env);

    // set model
    model.set(GRB_StringAttr_ModelName, "Minimizing Out-Degree");
    model.set(GRB_DoubleParam_TimeLimit, time_limit);
    model.set(GRB_DoubleParam_MIPGap, 0);

    // set decision variables for edges
    std::vector<GRBVar> edges(G.edge_count());
    for (auto& edgeVar : edges) {
      edgeVar = model.addVar(0.0, 1.0, 0, GRB_BINARY);
    }

    // add constraints
    GRBVar phi = model.addVar(0.0, 1.0, 0, GRB_INTEGER);  // std::numeric_limits<NodeID>::max();
    phi.set(GRB_DoubleAttr_UB, G.maxDegree());

    for (auto node : G.nodes()) {
      GRBLinExpr outDeg = 0;
      for (auto e : G.edgesFrom(node)) {
        if (G.edge(e)[0] == node) {
          outDeg += edges[e];  // sum up potential outgoing edges
        } else {
          outDeg += 1 - edges[e];
        }
      }
      model.addConstr(outDeg <= phi, "outDeg is bounded by phi");
    }

    // set objective
    model.setObjective(GRBLinExpr(phi), GRB_MINIMIZE);

    // optimize model
    model.optimize();

    // set solution
    isExact = model.get(GRB_IntAttr_Status) == GRB_OPTIMAL;
    for (auto e : G.edges()) {
      edge_orientation.SetOrientation(e, edges[e].get(GRB_DoubleAttr_X) > 0.5);
    }
    std::cout << "solution of ILP: " << phi.get(GRB_DoubleAttr_X) << std::endl;
    return absl::OkStatus();

  } catch (GRBException& e) {
    std::cout << "Gurobi Error: " << e.getMessage() << std::endl;
    exit(EXIT_FAILURE);
  }
#else
  return absl::UnimplementedError("Please compile with gurobi=enabled");
#endif
}
}  // namespace ilp
}  // namespace edge_orientation

}  // namespace rpo
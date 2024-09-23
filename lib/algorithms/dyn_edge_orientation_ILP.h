//
//
#ifdef USEILP
#ifndef DELTA_ORIENTATIONS_LOCALSEARCHILP_H
#define DELTA_ORIENTATIONS_LOCALSEARCHILP_H

#include <memory>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"
#include "priority_queues/bucket_pq.h"

class dyn_edge_orientation_ILP : public dyn_edge_orientation {
public:
  dyn_edge_orientation_ILP(
      const std::shared_ptr<dyn_graph_access> &GOrientation,
      const DeltaOrientationsConfig &config, DeltaOrientationsResult &result)
      : dyn_edge_orientation(GOrientation, config, result) {}

  void handleInsertion(NodeID source, NodeID target) override;
  void handleDeletion(NodeID source, NodeID target) override;
  void end() { solveILP(); };

  bool adjacent(NodeID source, NodeID target) { return true; }

private:
  void solveILP();
};

#endif // DELTA_ORIENTATIONS_LOCALSEARCHILP_H
#endif

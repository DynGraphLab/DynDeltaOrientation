//
// Created by Jannick Borowitz on 04.07.22.
// This class implements the normal and adaptive algorithm (unknown arboricity bound)
// of Brodal & Fagerberg "Representations for Sparse Graphs"
//

#ifndef DELTA_ORIENTATIONS_LOCALSEARCHBRODALFAGERBERG_H
#define DELTA_ORIENTATIONS_LOCALSEARCHBRODALFAGERBERG_H

#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"
#include "priority_queues/bucket_pq.h"

class dyn_edge_orientation_BrodalFagerberg : public dyn_edge_orientation {
        public:
                // Theorem 2 asserts that \Delta is equals 2*arboricityBound, since c is unknown we assume that c is at least 1 in the beginning
                dyn_edge_orientation_BrodalFagerberg(const std::shared_ptr<dyn_graph_access>& GOrientation, 
                                const DeltaOrientationsConfig& config,
                                DeltaOrientationsResult& result)
                        : dyn_edge_orientation(GOrientation, config, result),
                        GDirectedCopy(GOrientation->number_of_nodes()),
                        visitableNodes(),
                        reorientationsCounter(0),
                        reorientationsLimit(0),
                        delta(config.delta == 0 ? 2 : config.delta)
                        {}

                void handleInsertion(NodeID source, NodeID target) override;
                void handleDeletion(NodeID source, NodeID target) override;

                void end() {};

                bool adjacent(NodeID source, NodeID target) {return true;}
        protected:

                bool isBuilding = false;
                void build();

                dyn_graph_access GDirectedCopy;

                std::vector<NodeID> visitableNodes;

                EdgeID reorientationsCounter;
                EdgeID reorientationsLimit;
                EdgeWeight delta;
};

#endif  // DELTA_ORIENTATIONS_LOCALSEARCHBRODALFAGERBERG_H

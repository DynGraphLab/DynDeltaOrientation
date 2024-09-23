//
// Created by Jannick Borowitz on 29.04.22.
//

#ifndef DELTA_ORIENTATIONS_LOCALSEARCHKFLIPS_H
#define DELTA_ORIENTATIONS_LOCALSEARCHKFLIPS_H

#include <DeltaOrientationsConfig.h>
#include <DeltaOrientationsResult.h>

#include <memory>

#include "DynEdgeOrientation.h"
#include "priority_queues/bucket_pq.h"
#include "dyn_graph_access.h"

class dyn_edge_orientation_KFlips : public dyn_edge_orientation {
   public:
    dyn_edge_orientation_KFlips(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                      DeltaOrientationsResult& result);


    void handleInsertion(NodeID source, NodeID target) override;
    void handleDeletion(NodeID source, NodeID target) override;

    void end() {};

    bool adjacent(NodeID source, NodeID target) {
            for( unsigned i = 0; i < outNeighbors[source].size(); i++) {
                    if( outNeighbors[source][i] == target ) return true;
            }
            for( unsigned i = 0; i < outNeighbors[target].size(); i++) {
                    if( outNeighbors[target][i] == source ) return true;
            }

            return false;
    }
   private:
    void kFlips();

    bucket_pq outDegreeToNodes;

    // ToDo this could be moved into a special graph datastructure
    //  which maintains its adjacency lists along the FiFo-principle
    // we use deque because it needs to satisfy a queue and arbitrary deletions
    std::vector<std::deque<NodeID>> outNeighbors;
};

#endif  // DELTA_ORIENTATIONS_LOCALSEARCHKFLIPS_H

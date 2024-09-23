//
// Created by Jannick Borowitz on 29.04.22.
//

#include "dyn_edge_orientation_KFlips.h"
dyn_edge_orientation_KFlips::dyn_edge_orientation_KFlips(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                                     DeltaOrientationsResult& result)
    : dyn_edge_orientation(GOrientation, config, result),
      // bucket queue is reserving 10 buckets, e.q. is prepared for a max-out degree of 10 and resizes if necessary
      // later
      outDegreeToNodes(10),
      outNeighbors(GOrientation->number_of_nodes()) {
    for (NodeID node = 0; node < GOrientation->number_of_nodes(); ++node) {
        outDegreeToNodes.insert(node, GOrientation->getNodeDegree(node));
        forall_out_edges ((*GOrientation), e, node) {
            auto target = GOrientation->getEdgeTarget(node, e);
            outNeighbors[node].push_back(target);
        }
        endfor
    }
}

void dyn_edge_orientation_KFlips::kFlips() {
    auto k = config.flips;
    for (size_t i = 0; i < k; ++i) {
        auto source = outDegreeToNodes.maxElement();
        auto target = outNeighbors[source].front();
        outNeighbors[source].pop_front();
        GOrientation->remove_edge(source, target);
        GOrientation->new_edge(target, source);
        outNeighbors[target].push_back(source);
        outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
        outDegreeToNodes.changeKey(target, GOrientation->getNodeDegree(target));
    }
}

void dyn_edge_orientation_KFlips::handleInsertion(NodeID source, NodeID target) {
    GOrientation->new_edge(source, target);
    outNeighbors[source].push_back(target);
    outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
    kFlips();
}

void dyn_edge_orientation_KFlips::handleDeletion(NodeID source, NodeID target) {
    GOrientation->remove_edge(source, target);
    if(config.lsOnDeletion) {
        auto targetIt = std::find(outNeighbors[source].begin(), outNeighbors[source].end(), target);
        outNeighbors[source].erase(targetIt);
        outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
        kFlips();
    }

}

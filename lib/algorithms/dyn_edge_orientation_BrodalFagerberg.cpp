//
// Created by Jannick Borowitz on 04.07.22.
//

#include "dyn_edge_orientation_BrodalFagerberg.h"

void dyn_edge_orientation_BrodalFagerberg::build() {
        DELTAORI_LOG("build");
        std::cout <<  "TODO"  << std::endl;
        //isBuilding = true;

        //// clear graph
        //for (NodeID node = 0; node < GOrientation->number_of_nodes(); ++node) {
        //GOrientation->remove_node(node);
        //}
        //for (NodeID node = 0; node < GOrientation->number_of_nodes(); ++node) {
        //GOrientation->new_node(node);
        //}

        //// Todo reset components that are used in handleInsertion or handleDeletion

        //for (NodeID node = 0; node < GOrientation->number_of_nodes(); ++node) {
        //forall_out_edges (GDirectedCopy, e, node) {
        //auto target = GDirectedCopy.getEdgeTarget(node, e);
        //GOrientation->new_edge(node, target);
        //handleInsertion(node, target);
        //}
        //endfor
        //}

        //isBuilding = false;
}

void dyn_edge_orientation_BrodalFagerberg::handleInsertion(NodeID source, NodeID target) {
        GOrientation->new_edge(target, source); // maintain undirected graph
        if(config.delta == 0) {
                // adaptive variant needs to maintain some orientation of the current graph as copy

                //if (!GDirectedCopy.is_node(source)) {
                //GDirectedCopy.new_node(source);
                //}
                //if (!GDirectedCopy.is_node(target)) {
                //GDirectedCopy.new_node(target);
                //}
                DELTAORI_ASSERT(!GDirectedCopy.isEdge(source, target));
                GDirectedCopy.new_edge(source, target);
        }

        // use Theorem 2's amortized number of edge reorientations per edge
        // with \delta = \Delta / 2
        reorientationsLimit += delta + 1;

        DELTAORI_ASSERT(GOrientation->getNodeDegree(source) <= delta + 1);
        DELTAORI_ASSERT(visitableNodes.empty());

        if (GOrientation->getNodeDegree(source) == delta + 1) {
                visitableNodes.push_back(source);

                while (!visitableNodes.empty()) {
                        auto current = *std::prev(visitableNodes.end());
                        visitableNodes.pop_back();
                        forall_out_edges ((*GOrientation), e, current) {
                                auto neighbor = GOrientation->getEdgeTarget(current, e);
                                // create reverse edge
                                DELTAORI_ASSERT(!GOrientation->isEdge(neighbor, current));
                                GOrientation->new_edge(neighbor, current);
                                DELTAORI_ASSERT(GOrientation->getNodeDegree(target) <= delta + 1);

                                // does neighbor satisfy delta-orientation? If not visit target
                                if (GOrientation->getNodeDegree(neighbor) == delta + 1) {
                                        visitableNodes.push_back(neighbor);
                                }
                        }
                        endfor
                                // remove directed edges from source
                                while (GOrientation->getNodeDegree(current) > 0) {
                                        auto neighbor = GOrientation->getEdgeTarget(current, GOrientation->get_first_edge(current));
                                        DELTAORI_ASSERT(GOrientation->isEdge(current, neighbor));
                                        GOrientation->remove_edge(current, neighbor);
                                        ++reorientationsCounter;
                                }
                        if (reorientationsCounter > reorientationsLimit && config.delta == 0) {
                                // amortized limit of reorientations reached for current choice of Delta
                                // double delta and build graph again
                                delta = 2 * delta;
                                reorientationsCounter = 0;
                                reorientationsLimit = 0;
                                DELTAORI_ASSERT(!isBuilding);
                                visitableNodes.clear();
                                build();
                                return;
                        }
                }
        }

}
void dyn_edge_orientation_BrodalFagerberg::handleDeletion(NodeID source, NodeID target) {
        GOrientation->remove_edge(source, target);
        if (GDirectedCopy.isEdge(source, target)) {
                GDirectedCopy.remove_edge(source, target);
        } else {
                DELTAORI_ASSERT(GDirectedCopy.isEdge(target, source));
                GDirectedCopy.remove_edge(target, source);
        }


}

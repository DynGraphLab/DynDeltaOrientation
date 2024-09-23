//
// Created by Jannick Borowitz on 01.07.22.
//
// This file contains algorithms that use a DFS in order to find improving paths.
//

#include <DeltaOrientationsConfig.h>
#include <DeltaOrientationsResult.h>

#include <memory>
#include <stack>

#include "dyn_graph_access.h"
#include "DynEdgeOrientation.h"
#include "priority_queues/bucket_pq.h"

#ifndef DELTA_ORIENTATIONS_LOCALSEARCHDFS_H
#define DELTA_ORIENTATIONS_LOCALSEARCHDFS_H

class dyn_edge_orientation_DFS : public dyn_edge_orientation {
   public:
    dyn_edge_orientation_DFS(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config, DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result),
          parent(GOrientation->number_of_nodes(), PARENT_ROOT),
          expanded(GOrientation->number_of_nodes(), NODE_DFS_COUNTER_INIT),
          outDegreeToNodes(10) {
        for (NodeID node = 0; node < GOrientation->number_of_nodes(); ++node) {
            //if (GOrientation->is_node(node)) outDegreeToNodes.insert(node, GOrientation->getNodeDegree(node));
            outDegreeToNodes.insert(node, GOrientation->getNodeDegree(node));
        }

        // track out degrees in case of the following configuration
        trackOutDegrees =
            config.stackDFS || (config.recursiveDFS && (config.dfsDepthBound == 0 || config.dfsBranchBound == 0));
    }

    void handleInsertion(NodeID source, NodeID target) override;

    void end() {};

    bool adjacent(NodeID source, NodeID target) {return true;}
    /**
     * Performs a DFS and randomly selects the next neighbor that is explored using a recursive implementation to find
     * an improving path.
     * @param source
     * @param delta
     */
    void findImprovingPathWithRecursiveDFS(const NodeID& source, const NodeWeight& delta) {
        if (findImprovingPathWithRecursiveDFSImpl(source, delta)) {
            outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
        }
    }

    bool findImprovingPathWithRecursiveDFSImpl(const NodeID& current, const NodeWeight& delta);

    /**
     * Performs a DFS and randomly selects the next neighbor to visit using a none-recursive implementation to find an
     * improving path.
     * @param source
     * @param delta
     */
    void findImprovingPathWithDFS(NodeID source, NodeWeight delta);

    /**
     * Performs a branch and depth bounded DFS that selects the next neighbor to visit randomly using a recursive
     * implementation.
     * @param source
     * @param delta
     * @param branchBound
     * @param maxDepth
     */
    void findImprovingPathWithRecursiveBoundedDFS(const NodeID& source, const NodeWeight& delta, const EdgeID& branchBound,
                                       EdgeID maxDepth = std::numeric_limits<EdgeID>::max()) {
        if (findImprovingPathWIthRecursiveBoundedDFSImpl(source, delta, branchBound, maxDepth)) {
            outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
        }
    }

    bool findImprovingPathWIthRecursiveBoundedDFSImpl(const NodeID& current, const NodeWeight& delta, const EdgeID& branchBound,
                                           EdgeID maxDepth);
    void handleDeletion(NodeID source, NodeID target) override;

   protected:
    std::vector<NodeID> parent;
    const NodeID PARENT_ROOT = std::numeric_limits<NodeID>::max();

    std::vector<int> expanded;

    const int NODE_DFS_COUNTER_INIT = -1;
    int dfsCounter = NODE_DFS_COUNTER_INIT + 1;

    bucket_pq outDegreeToNodes;

    bool trackOutDegrees;

    void flipEdge(NodeID source, NodeID target) {
        DELTAORI_ASSERT(GOrientation->isEdge(source, target));
        DELTAORI_ASSERT(!GOrientation->isEdge(target, source));

        GOrientation->remove_edge(source, target);
        GOrientation->new_edge(target, source);

        DELTAORI_ASSERT(!GOrientation->isEdge(source, target));
        DELTAORI_ASSERT(GOrientation->isEdge(target, source));
    }

    void flipPath(NodeID target) {
        DELTAORI_ASSERT(target != PARENT_ROOT);

        // target's outdegree increases by one
        outDegreeToNodes.changeKey(target, GOrientation->getNodeDegree(target) + 1);
        // flip edges
        while (parent[target] != PARENT_ROOT) {
            auto p = parent[target];
            flipEdge(p, target);
            target = p;
        }
        // update source's outdegree (decreased by one)
        outDegreeToNodes.changeKey(target, GOrientation->getNodeDegree(target));
    }

    bool isSingleMaxDegreeNode(NodeID node){
        DELTAORI_ASSERT(trackOutDegrees);

        return outDegreeToNodes.getKey(node) == outDegreeToNodes.maxValue() &&
               outDegreeToNodes.getBucketByKey(outDegreeToNodes.maxValue()).size() == 1;
    }
};

#endif  // DELTA_ORIENTATIONS_LOCALSEARCHDFS_H

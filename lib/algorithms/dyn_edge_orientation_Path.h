//
// Created by Jannick Borowitz on 24.04.22.
//

#ifndef DELTA_ORIENTATIONS_LOCALSEARCHPATH_H
#define DELTA_ORIENTATIONS_LOCALSEARCHPATH_H

#include <memory>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"
#include "priority_queues/bucket_pq.h"

class dyn_edge_orientation_Path : public dyn_edge_orientation {
   public:
    dyn_edge_orientation_Path(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                    DeltaOrientationsResult& result);

    void handleInsertion(NodeID source, NodeID target) override;
    void handleDeletion(NodeID source, NodeID target) override;
    void end() {};

    bool adjacent(NodeID source, NodeID target) {return true;}
   private:
    const int NODE_BFS_COUNTER_INIT = -1;
    const int PARENT_ROOT = 0;

    // we maintain GTransposed for bidirectional BFS and for local-search at deletion
    dyn_graph_access GTransposed;

    int bfsCounter = NODE_BFS_COUNTER_INIT + 1;
    std::queue<NodeID> sourceQ;
    std::queue<NodeID> targetQ;
    std::vector<int> sourceQueued;
    std::vector<int> targetQueued;

    std::vector<NodeID> sourceParent;
    std::vector<NodeID> targetParent;

    bucket_pq outDegreeToNodes;

    /**
     * Find an improving path and return the intersection between source and an feasible target.
     * @param source
     * @param isTarget
     * @param bfsCapacity
     * @param targetBFSThreshold
     * @param initTargetBFS
     * @return intersection or std::numeric_limits<NodeID>::max() if no intersection was found
     */
    NodeID findImprovingPath(
        NodeID source, const std::function<bool(NodeID)>& isTarget,
        dyn_graph_access&G, dyn_graph_access &GRev,
        NodeID bfsCapacity = 0,
        NodeID targetBFSThreshold = std::numeric_limits<NodeID>::max(),
        const std::function<void(NodeID&)>& initTargetBFS = [](NodeID& currentBFSCapacity) {});

    void flip(NodeID source, NodeID intersection, dyn_graph_access &G, dyn_graph_access &GRev) {
        NodeID intersectionCopy = intersection;

        while (targetParent[intersectionCopy] != intersectionCopy) {
            G.remove_edge(intersectionCopy, targetParent[intersectionCopy]);
            G.new_edge(targetParent[intersectionCopy], intersectionCopy);
            GRev.remove_edge(targetParent[intersectionCopy], intersectionCopy);
            GRev.new_edge(intersectionCopy, targetParent[intersectionCopy]);
            intersectionCopy = targetParent[intersectionCopy];
        }
        while (source != intersection) {
            DELTAORI_ASSERT(G.isEdge(sourceParent[intersection], intersection));
            DELTAORI_ASSERT(GRev.isEdge(intersection, sourceParent[intersection]));

            G.remove_edge(sourceParent[intersection], intersection);
            G.new_edge(intersection, sourceParent[intersection]);
            GRev.remove_edge(intersection, sourceParent[intersection]);
            GRev.new_edge(sourceParent[intersection], intersection);
            intersection = sourceParent[intersection];
        }
        updateDegree(intersectionCopy);
        updateDegree(source);

    }

    void updateDegree(NodeID node) {
        outDegreeToNodes.changeKey(node, GOrientation->getNodeDegree(node));
        DELTAORI_ASSERT(outDegreeToNodes.getKey(node) == GOrientation->getNodeDegree(node));
    }

    void initTargetBFS(NodeID delta, NodeID& bfsCapacity) {
        int counter = 0;
        int maxNodesAtInit = 0;
        // maxNodesAtInit = static_cast<int>(0.001 * potentialEndpoints);  //
        // GOrientation->capacity_of_nodes()/100; maxNodesAtInit = 0;
        for (NodeWeight outDeg = 0; outDeg < delta; ++outDeg) {
            if (bfsCapacity > 0 && counter < maxNodesAtInit) {
                for (const auto& node : outDegreeToNodes.getBucketByKey(outDeg)) {
                    if (bfsCapacity > 0 && counter < maxNodesAtInit) {
                        DELTAORI_ASSERT(!wasQueued(node));
                        targetQ.push(node);
                        targetParent[node] = node;
                        targetQueued[node] = bfsCounter;
                        --bfsCapacity;
                        ++counter;
                    }
                }
            }
        }
        DELTAORI_DEBUG(DELTAORI_LOG("maxNodesAtInit: " << maxNodesAtInit));
        DELTAORI_DEBUG(DELTAORI_LOG(counter));
    }

    void resetBFS() {
        // reset bfs
        while (!sourceQ.empty()) {
            sourceQ.pop();
        }
        while (!targetQ.empty()) {
            targetQ.pop();
        }
        ++bfsCounter;
    }
    bool wasQueued(NodeID node) { return sourceQueued[node] == bfsCounter; }
    bool wasQueued2(NodeID node) { return targetQueued[node] == bfsCounter; }

    bool isSingleMaxDegreeNode(NodeID node){
        return outDegreeToNodes.getKey(node) == outDegreeToNodes.maxValue() &&
             outDegreeToNodes.getBucketByKey(outDegreeToNodes.maxValue()).size() == 1;
    }

    void obtainStatisticsForBFS(NodeID nodeCounterDESC) {
        DELTAORI_LOG("nodes expanded by bfs: " << (config.bfsMaxNodes == 0
                                                      ? std::numeric_limits<unsigned>::max() - nodeCounterDESC
                                                      : config.bfsMaxNodes - nodeCounterDESC));

        result.geoMeanVisitedNodes +=
            log(config.bfsMaxNodes == 0 ? std::numeric_limits<unsigned>::max() - nodeCounterDESC
                                          : config.bfsMaxNodes - nodeCounterDESC);
        result.performedBFS += 1;
    }

    //void localSearchOnInsertionUsingHeuristic(NodeID source, NodeID target);
};

#endif  // DELTA_ORIENTATIONS_LOCALSEARCHPATH_H

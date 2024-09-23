//
// Created by Jannick Borowitz on 24.04.22.
//

#include <cmath>
#include "dyn_edge_orientation_Path.h"
#include "random_functions.h"

dyn_edge_orientation_Path::dyn_edge_orientation_Path(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result),
        GTransposed(GOrientation->number_of_nodes()),
        sourceQueued(GOrientation->number_of_nodes(), NODE_BFS_COUNTER_INIT),
        targetQueued(GOrientation->number_of_nodes(), NODE_BFS_COUNTER_INIT),
        sourceParent(GOrientation->number_of_nodes(), PARENT_ROOT),
        targetParent(GOrientation->number_of_nodes(), PARENT_ROOT),

        //TODO CS check n
        // reserve 10 buckets; if more needed later, we allocate in amortized O(1)
        outDegreeToNodes(GOrientation->number_of_nodes())  {
        forall_nodes((*GOrientation), node) {
                outDegreeToNodes.insert(node, 0);
        } endfor
}

void dyn_edge_orientation_Path::handleInsertion(NodeID source, NodeID target) {
        // maintain transposed graph and out-degree-to-vertices map
        //bool insertSource = false;
        //if (!GTransposed.is_node(source)) {
                //insertSource = true;
                //GTransposed.new_node(source);
                //outDegreeToNodes.insert(source, 1);
        //}
        //if (!GTransposed.is_node(target)) {
                //GTransposed.new_node(target);
                //outDegreeToNodes.insert(target, 0);
        //}

        GOrientation->new_edge(source, target);
        GTransposed.new_edge(target, source);
        
        //if (!insertSource) {
        outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
        //}

        // improvement goal for source-y-path
        auto delta = static_cast<long>(GOrientation->getNodeDegree(source)) - 1;
        if (delta <= 0) {
                DELTAORI_DEBUG(DELTAORI_LOG("Local search was not possible, since delta<=0"));
                return;
        }

        DELTAORI_DEBUG(DELTAORI_LOG("Starting local search with delta=" << delta));

        DELTAORI_ASSERT(outDegreeToNodes.maxValue() > 0);  // inequality must hold since out-degree of source is at least 1
        if (outDegreeToNodes.maxValue() - 1 <= outDegreeToNodes.minValue()) {
                DELTAORI_LOG("no update necessary because there cannot be an improving path");
                return;
        }

        // is bfs bounded?
        NodeID bfsCapacity;
        if (config.bfsMaxNodes == 0) {
                // no bound on the nodes in the bfs
                bfsCapacity = std::numeric_limits<NodeID>::max();

                // check whether a search for an improving path could improve the current orientation
                if (!isSingleMaxDegreeNode(source)) {
                        DELTAORI_LOG("no update necessary");
                        return;
                }
        } else {
                // Note: we don't check whether source is a single max degree node

                bfsCapacity = config.bfsMaxNodes;
        }

        // if targetBFSThreshold is set to max, we will not perform a bidirectional bfs
        NodeID targetBFSThreshold = std::numeric_limits<NodeID>::max();

        auto initTargetBFSForDelta = [&solver = *this, &delta](NodeID& currentBFSCapacity) {
                solver.initTargetBFS(delta, currentBFSCapacity);
        };

        auto isTarget = [&G = GOrientation, &delta](NodeID node) { return G->getNodeDegree(node) < delta; };

        // intersection is feasible target if no bidirectional bfs was used
        NodeID intersection = findImprovingPath(source, isTarget, *GOrientation, GTransposed, bfsCapacity,
                        targetBFSThreshold, initTargetBFSForDelta);


        if (intersection != std::numeric_limits<NodeID>::max()) {
                flip(source, intersection, *GOrientation, GTransposed);
        } else {
                DELTAORI_LOG("found no improving path");
        }

        // update result statistic
        if (result.largestMaxOutDeg < outDegreeToNodes.maxValue()) {
                result.largestMaxOutDeg = outDegreeToNodes.maxValue();
        }
}

void dyn_edge_orientation_Path::handleDeletion(NodeID source, NodeID target) {
        //DELTAORI_ASSERT(GTransposed.is_node(source));
        //DELTAORI_ASSERT(GTransposed.is_node(target));
        //DELTAORI_ASSERT(GTransposed.isEdge(target, source));
        //DELTAORI_ASSERT(GOrientation->getNodeDegree(target) == outDegreeToNodes.getKey(target));

        GOrientation->remove_edge(target, source);
        GTransposed.remove_edge(target, source);
        outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));

        if (!config.lsOnDeletion) return;

        // to which delta do we want to prune the maximum out-degree?
        auto delta = static_cast<long>(outDegreeToNodes.maxValue()) - 1;

        if (delta <= 0 || outDegreeToNodes.minValue() >= outDegreeToNodes.maxValue() - 1) {
                DELTAORI_DEBUG(DELTAORI_LOG("Local search was not possible, since delta<=0"));
                return;
        }

        DELTAORI_DEBUG(DELTAORI_LOG("Starting local search with delta=" << delta));

        NodeID bfsCapacity = std::numeric_limits<NodeID>::max();
        if (config.bfsMaxNodes > 0) {
                bfsCapacity = config.bfsMaxNodes;
        }

        // is source an improving target or do we search for an improving target?
        if (GOrientation->getNodeDegree(source) < delta) {
                // search for an improving x-source-path
                // bfs in GTransposed
                auto initTargetBFSForDelta = [&solver = *this, &delta](NodeID& currentBFSCapacity) {
                        solver.initTargetBFS(delta, currentBFSCapacity);
                };

                auto isTarget = [&G = GOrientation, &delta](NodeID node) { return G->getNodeDegree(node) > delta; };

                NodeID targetBFSThreshold = std::numeric_limits<NodeID>::max();

                NodeID intersection = findImprovingPath(source, isTarget, GTransposed, *GOrientation, bfsCapacity,
                                targetBFSThreshold, initTargetBFSForDelta);

                if (intersection != std::numeric_limits<NodeID>::max()) {
                        DELTAORI_ASSERT(GOrientation->getNodeDegree(intersection) > delta);
                        flip(source, intersection, GTransposed, *GOrientation);
                } else {
                        DELTAORI_LOG("found no improving path");
                }

        } else if (GOrientation->getNodeDegree(source) == delta) {
                // search for an improving x-y-path
                // bfs in GOrientation for some random source

                auto& feasibleTargets = outDegreeToNodes.getBucketByKey(delta + 1);

                auto bfsSource = feasibleTargets[random_functions::nextInt(0, feasibleTargets.size() - 1)];

                auto initTargetBFSForDelta = [&solver = *this, &delta](NodeID& currentBFSCapacity) {
                        solver.initTargetBFS(delta, currentBFSCapacity);
                };

                auto isTarget = [&G = GOrientation, &delta](NodeID node) { return G->getNodeDegree(node) < delta; };

                NodeID targetBFSThreshold = std::numeric_limits<NodeID>::max();

                NodeID intersection = findImprovingPath(bfsSource, isTarget, *GOrientation, GTransposed, bfsCapacity,
                                targetBFSThreshold, initTargetBFSForDelta);

                if (intersection != std::numeric_limits<NodeID>::max()) {
                        DELTAORI_ASSERT(GOrientation->getNodeDegree(intersection) < delta);
                        flip(bfsSource, intersection, *GOrientation, GTransposed);
                } else {
                        DELTAORI_LOG("found no improving path");
                }

        } else {
                // search for an improving source-y-path
                // bfs in GOrientation for source

                auto initTargetBFSForDelta = [&solver = *this, &delta](NodeID& currentBFSCapacity) {
                        solver.initTargetBFS(delta, currentBFSCapacity);
                };

                auto isTarget = [&G = GOrientation, &delta](NodeID node) { return G->getNodeDegree(node) < delta; };

                NodeID targetBFSThreshold = std::numeric_limits<NodeID>::max();

                NodeID intersection = findImprovingPath(source, isTarget, *GOrientation, GTransposed, bfsCapacity,
                                targetBFSThreshold, initTargetBFSForDelta);

                if (intersection != std::numeric_limits<NodeID>::max()) {
                        DELTAORI_ASSERT(GOrientation->getNodeDegree(intersection) < delta);
                        flip(source, intersection, *GOrientation, GTransposed);
                } else {
                        DELTAORI_LOG("found no improving path");
                }
        }

}

NodeID dyn_edge_orientation_Path::findImprovingPath(NodeID source, const std::function<bool(NodeID)>& isTarget, dyn_graph_access& G,
                dyn_graph_access& GRev, NodeID bfsCapacity, NodeID targetBFSThreshold,
                const std::function<void(NodeID&)>& initTargetBFS) {
        sourceQ.push(source);
        sourceQueued[source] = bfsCounter;
        sourceParent[source] = PARENT_ROOT;
        --bfsCapacity;

        unsigned nodesInBfsQueueAtCurrentDepth;
        unsigned nodesInBfsQueue2AtCurrentDepth;

        int countExploredNodesInFirstBFS = 0;
        while (!sourceQ.empty() || !targetQ.empty()) {
                nodesInBfsQueueAtCurrentDepth = sourceQ.size();
                nodesInBfsQueue2AtCurrentDepth = targetQ.size();
                while (nodesInBfsQueueAtCurrentDepth > 0) {
                        auto current = sourceQ.front();
                        sourceQ.pop();
                        --nodesInBfsQueueAtCurrentDepth;

                        if (isTarget(current)) {
                                // stop earlier
                                targetParent[current] = current;
                                resetBFS();
                                obtainStatisticsForBFS(bfsCapacity);
                                return current;
                        }

                        forall_out_edges (G, e, current) {
                                auto target2 = G.getEdgeTarget(current, e);

                                if (!wasQueued(target2) && bfsCapacity > 0) {
                                        if (wasQueued2(target2)) {
                                                // we found a path
                                                sourceParent[target2] = current;
                                                DELTAORI_LOG("intersection");
                                                resetBFS();
                                                obtainStatisticsForBFS(bfsCapacity);
                                                return target2;
                                        } else {
                                                sourceQ.push(target2);
                                                sourceQueued[target2] = bfsCounter;
                                                sourceParent[target2] = current;
                                                --bfsCapacity;
                                                if (countExploredNodesInFirstBFS < targetBFSThreshold) {
                                                        ++countExploredNodesInFirstBFS;
                                                        if (countExploredNodesInFirstBFS == targetBFSThreshold) {
                                                                // initialize second bfs
                                                                initTargetBFS(bfsCapacity);
                                                        }
                                                }
                                        }
                                }
                        }
                        endfor
                }
                // if we run a bidirectional bfs, then visit its next level
                if (countExploredNodesInFirstBFS >= targetBFSThreshold) {
                        while (nodesInBfsQueue2AtCurrentDepth > 0) {
                                auto current = targetQ.front();
                                targetQ.pop();
                                --nodesInBfsQueue2AtCurrentDepth;

                                forall_out_edges (GRev, e, current) {
                                        auto target2 = GRev.getEdgeTarget(current, e);

                                        if (!wasQueued2(target2) && bfsCapacity > 0) {
                                                if (wasQueued(target2)) {
                                                        targetParent[target2] = current;
                                                        DELTAORI_LOG("intersection second bfs");
                                                        resetBFS();
                                                        obtainStatisticsForBFS(bfsCapacity);
                                                        return target2;
                                                } else {
                                                        targetQ.push(target2);
                                                        targetQueued[target2] = bfsCounter;
                                                        targetParent[target2] = current;
                                                        --bfsCapacity;
                                                }
                                        }
                                }
                                endfor
                        }
                } else if (sourceQ.empty()) {
                        // no improvement possible
                        resetBFS();
                        obtainStatisticsForBFS(bfsCapacity);
                        return std::numeric_limits<NodeID>::max();
                }
        }

        resetBFS();
        obtainStatisticsForBFS(bfsCapacity);
        return std::numeric_limits<NodeID>::max();
}

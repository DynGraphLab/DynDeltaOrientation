//
// Created by Jannick Borowitz on 01.07.22.
//

#include "dyn_edge_orientation_DFS.h"

#include "random_functions.h"

void dyn_edge_orientation_DFS::handleInsertion(NodeID source, NodeID target) {

    GOrientation->new_edge(target, source); // maintain undirected graph
    if (!trackOutDegrees) return;

    if (outDegreeToNodes.contains(source)) {
        outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
    } else {
        outDegreeToNodes.insert(source, GOrientation->getNodeDegree(source));
    }
    if (!outDegreeToNodes.contains(target)) {
        outDegreeToNodes.insert(target, 0);
    }

    auto delta = static_cast<long>(GOrientation->getNodeDegree(source)) - 1;
    if (delta <= 0) {
        DELTAORI_DEBUG(DELTAORI_LOG("Local search was not possible, since delta<=0"));
        return;
    }

    DELTAORI_ASSERT(outDegreeToNodes.maxValue() > 0);  // inequality must hold since out-degree of source is at least 1
    if(outDegreeToNodes.maxValue() - 1 <= outDegreeToNodes.minValue()) {
        DELTAORI_DEBUG(DELTAORI_LOG("no update necessary because there cannot be an improving path"));
        return;
    }

    DELTAORI_DEBUG(DELTAORI_LOG("Starting local search with delta=" << delta));

    if (trackOutDegrees) {
        // check whether an improving path from source can decrease the maximum out-degree
        if (!isSingleMaxDegreeNode(source)) {
            DELTAORI_LOG("no update necessary");
            return;
        }

        if (config.recursiveDFS) {
            findImprovingPathWithRecursiveDFS(source, delta);
        } else if (config.stackDFS) {
            findImprovingPathWithDFS(source, delta);
        } else {
            throw std::invalid_argument("no exact dfs algorithm selected.");
        }
    } else {
        if (config.recursiveDFS) {
            unsigned dfsBranchBound = config.dfsBranchBound;
            unsigned dfsDepthBound = config.dfsDepthBound;
            if (dfsBranchBound == 0) {
                dfsBranchBound = std::numeric_limits<unsigned>::max();
            } else if (dfsDepthBound == 0) {
                dfsDepthBound = std::numeric_limits<unsigned>::max();
            }
            findImprovingPathWithRecursiveBoundedDFS(source, delta, dfsBranchBound, dfsDepthBound);
        } else {
            throw std::invalid_argument("no in-exact rw algorithm selected.");
        }
    }

    ++dfsCounter;
}

void dyn_edge_orientation_DFS::handleDeletion(NodeID source, NodeID target) {
    GOrientation->remove_edge(source, target);
    outDegreeToNodes.changeKey(source, GOrientation->getNodeDegree(source));
}

bool dyn_edge_orientation_DFS::findImprovingPathWIthRecursiveBoundedDFSImpl(const NodeID& current, const NodeWeight& delta,
                                                                  const EdgeID& branchBound, EdgeID maxDepth) {
    expanded[current] = dfsCounter;
    std::vector<NodeID> neighbors;
    neighbors.reserve(GOrientation->getNodeDegree(current));
    forall_out_edges ((*GOrientation), e, current) {
        auto target = GOrientation->getEdgeTarget(current, e);
        if (expanded[target] != dfsCounter) {
            if (GOrientation->getNodeDegree(target) < delta) {
                // success
                flipEdge(current, target);
                if (trackOutDegrees) outDegreeToNodes.changeKey(target, GOrientation->number_of_nodes());
                return true;
            }

            if (maxDepth > 1) {
                neighbors.push_back(target);
            }
        }
    }
    endfor

    // invariant: vector "neighbors" contains all neighbors of current which were not expanded by another call
    // invariant: every node v that is marked "expanded" is no feasible endpoint of an improving path

    if (maxDepth <= 1) {
        return false;
    }

    random_functions::permutate_vector_fast(neighbors, false);

    return std::any_of(
        neighbors.begin(), branchBound < neighbors.size() ? neighbors.begin() + branchBound : neighbors.end(),
        [&solver = *this, &current, &delta, &branchBound, &maxDepth](const NodeID& target) {
            if (solver.expanded[target] != solver.dfsCounter) {
                if (solver.findImprovingPathWIthRecursiveBoundedDFSImpl(target, delta, branchBound, maxDepth - 1)) {
                    solver.flipEdge(current, target);
                    return true;
                }
            }
            return false;
        });
}
bool dyn_edge_orientation_DFS::findImprovingPathWithRecursiveDFSImpl(const NodeID& current, const NodeWeight& delta) {
    DELTAORI_ASSERT(expanded[current] != dfsCounter);
    DELTAORI_ASSERT(GOrientation->getNodeDegree(current) >= delta);
    expanded[current] = dfsCounter;
    std::vector<NodeID> neighbors;
    neighbors.reserve(GOrientation->getNodeDegree(current));
    forall_out_edges ((*GOrientation), e, current) {
        auto target = GOrientation->getEdgeTarget(current, e);
        if (expanded[target] != dfsCounter) {
            if (GOrientation->getNodeDegree(target) < delta) {
                // success
                flipEdge(current, target);
                if (trackOutDegrees) outDegreeToNodes.changeKey(target, GOrientation->getNodeDegree(target));
                return true;
            }

            neighbors.push_back(target);
        }
    }
    endfor

    // invariant: vector "neighbors" contains all neighbors of current which were not expanded by another call
    // invariant: every node v that is marked "expanded" is no feasible endpoint of an improving path

    // random_functions::permutate_vector_fast(neighbors, false);
    std::sort(neighbors.begin(), neighbors.end(),
              [&expanded = expanded](NodeID first, NodeID second) { return expanded[first] < expanded[second]; });

    return std::any_of(neighbors.begin(), neighbors.end(), [&solver = *this, &current, &delta](const NodeID& target) {
        if (solver.expanded[target] != solver.dfsCounter) {
            if (solver.findImprovingPathWithRecursiveDFSImpl(target, delta)) {
                solver.flipEdge(current, target);
                return true;
            }
        }
        return false;
    });
}
void dyn_edge_orientation_DFS::findImprovingPathWithDFS(NodeID source, NodeWeight delta) {
    std::stack<NodeID, std::vector<NodeID>> sourceS;
    sourceS.push(source);
    parent[source] = PARENT_ROOT;

    while (!sourceS.empty()) {
        const NodeID current = sourceS.top();
        sourceS.pop();

        if (expanded[current] == dfsCounter) {
            continue;
        } else {
            expanded[current] = dfsCounter;  // expanded if popped from stack
        }

        // DELTAORI_LOG("explore");

        std::vector<NodeID> neighbors;
        neighbors.reserve(GOrientation->getNodeDegree(current));

        forall_out_edges ((*GOrientation), e, current) {
            auto target = GOrientation->getEdgeTarget(current, e);
            if (expanded[target] != dfsCounter) {
                parent[target] = current;

                if (GOrientation->getNodeDegree(target) < delta) {
                    // success
                    flipPath(target);
                    DELTAORI_LOG("found improving path");
                    return;
                }

                neighbors.push_back(target);
            }
        }
        endfor

        // random_functions::permutate_vector_fast(neighbors, false);
        std::sort(neighbors.begin(), neighbors.end(),
                  [&expanded = expanded](NodeID first, NodeID second) { return expanded[first] < expanded[second]; });

        std::for_each(neighbors.begin(), neighbors.end(), [&sourceS](NodeID target) { sourceS.push(target); });
    }
}

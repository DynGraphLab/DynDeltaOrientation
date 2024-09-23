//
// Created by Jannick Borowitz on 29.05.22.
//

#include "dyn_edge_orientation_RWalk.h"

void dyn_edge_orientation_RWalk::randomWalk(const NodeID& source, const NodeWeight& delta, const int maxWalks,
                                  const EdgeID maxLength) {
    // ToDo if walk can be long, make path a member of the class
    std::vector<NodeID> path(maxLength + 1, 0);
    path[0] = source;

    edges.clear();
    appearancesAsSource.clear();

    // until maxWalks random walks were performed or a successful walk was done
    for (size_t walk = 0; walk < maxWalks; ++walk) {
        appearancesAsSource[source] = 1;
        // we search for a walk with at most maxLength edges
        for (NodeID currentPos = 0; currentPos < maxLength; ++currentPos) {
            EdgeID e;
            NodeID target;

            // select next edge randomly
            DELTAORI_ASSERT(GOrientation->getNodeDegree(path[currentPos]) > appearancesAsSource[path[currentPos]] - 1);
            do {
                DELTAORI_LOG(GOrientation->get_first_edge(path[currentPos]));
                DELTAORI_LOG(GOrientation->get_first_invalid_edge(path[currentPos]) - 1);
                e = random_functions::nextInt(GOrientation->get_first_edge(path[currentPos]),
                                              GOrientation->get_first_invalid_edge(path[currentPos]) - 1);
                target = GOrientation->getEdgeTarget(path[currentPos], e);
            } while (edges.find(std::make_pair(path[currentPos], target)) != edges.end());

            path[currentPos + 1] = target;
            edges.insert(std::make_pair(path[currentPos], target));

            if (appearancesAsSource.find(target) != appearancesAsSource.end()) {
                // target appears for at least the second time in the path (and thus not a feasible end of the walk)
                // if out-degree of target is equals to the number of appearances as source, then we know every edge is
                // covered
                if (GOrientation->getNodeDegree(target) == appearancesAsSource[target]) {
                    // there is now new edge
                    //  thus reset path
                    edges.clear();
                    appearancesAsSource.clear();
                    break;
                }

                ++appearancesAsSource[target];
            } else {
                // target appears for the first time in the walk
                // is target a feasible target?
                if (GOrientation->getNodeDegree(target) < delta) {
                    // found improving walk
                    flipPathPrefixUntilPos(path, currentPos+1);
                    return;
                }

                appearancesAsSource[target] = 1;
            }
        }
        edges.clear();
        appearancesAsSource.clear();
    }
}

void dyn_edge_orientation_RWalk::handleInsertion(NodeID source, NodeID target) {
    GOrientation->new_edge(source, target);
    // improvement goal for random walk (walk is improving if last vertex has out-degree less than delta)
    auto delta = static_cast<long>(GOrientation->getNodeDegree(source)) - 1;
    if (delta <= 0) {
        DELTAORI_DEBUG(DELTAORI_LOG("Local search was not possible, since delta<=0"));
        return;
    }

    DELTAORI_DEBUG(DELTAORI_LOG("Starting local search with delta=" << delta));

    randomWalk(source, delta, static_cast<signed>(config.rwAmount), config.rwLengthBound);
}

void dyn_edge_orientation_RWalk::handleDeletion(NodeID source, NodeID target) {
    GOrientation->remove_edge(source, target);
    if(!config.lsOnDeletion) {
        return;
    }

    // improvement goal for random walk (walk is improving if last vertex has out-degree less than delta)
    auto delta = static_cast<long>(GOrientation->getNodeDegree(source)) - 1;
    if (delta <= 0) {
        DELTAORI_DEBUG(DELTAORI_LOG("Local search was not possible, since delta<=0"));
        return;
    }

    DELTAORI_DEBUG(DELTAORI_LOG("Starting local search with delta=" << delta));

    randomWalk(source, delta, static_cast<signed>(config.rwAmount), config.rwLengthBound);
}

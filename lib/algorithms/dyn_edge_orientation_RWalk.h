//
// Created by Jannick Borowitz on 29.05.22.
//

#ifndef DELTA_ORIENTATIONS_LOCALSEARCHRWALK_H
#define DELTA_ORIENTATIONS_LOCALSEARCHRWALK_H

#include <algorithm>
#include <sparsehash/dense_hash_map>
#include <sparsehash/dense_hash_set>
#include <stack>
#include <unordered_set>

#include "DynEdgeOrientation.h"
#include "priority_queues/bucket_pq.h"
#include "random_functions.h"
#include "tools/more_hasher.h"

class dyn_edge_orientation_RWalk : public dyn_edge_orientation {
        public:
                dyn_edge_orientation_RWalk(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                                DeltaOrientationsResult& result)
                        : dyn_edge_orientation(GOrientation, config, result) {
                                edges.set_deleted_key(std::make_pair(std::numeric_limits<NodeID>::max(), std::numeric_limits<NodeID>::max()));
                                edges.set_empty_key(std::make_pair(std::numeric_limits<NodeID>::max()-1, std::numeric_limits<NodeID>::max()-1));
                                edges.min_load_factor(0.0);
                                appearancesAsSource.set_deleted_key(std::numeric_limits<int>::max());
                                appearancesAsSource.set_empty_key(std::numeric_limits<int>::max() - 1);
                                appearancesAsSource.min_load_factor(0.0);
                        }

                void randomWalk(const NodeID& source, const NodeWeight& delta, int maxWalks = 20,
                                EdgeID maxLength = 20);
                void handleInsertion(NodeID source, NodeID target) override;
                void handleDeletion(NodeID source, NodeID target) override;
                void end() {};

                bool adjacent(NodeID source, NodeID target) {
                        return GOrientation->isEdge(source, target) || GOrientation->isEdge(target, source) ;
                }
        protected:
                // for random walks
                google::dense_hash_set<std::pair<NodeID, NodeID>, std::hash<std::pair<NodeID, NodeID>>> edges;
                google::dense_hash_map<NodeID, int> appearancesAsSource;

                void flipEdge(NodeID source, NodeID target) {
                        DELTAORI_ASSERT(GOrientation->isEdge(source, target));
                        DELTAORI_ASSERT(!GOrientation->isEdge(target, source));

                        GOrientation->remove_edge(source, target);
                        GOrientation->new_edge(target, source);

                        DELTAORI_ASSERT(!GOrientation->isEdge(source, target));
                        DELTAORI_ASSERT(GOrientation->isEdge(target, source));
                }

                void flipPathPrefixUntilPos(const std::vector<NodeID> &path, size_t endPrefixPos) {
                        for (NodeID flipPos = 1; flipPos <= endPrefixPos; ++flipPos) {
                                flipEdge(path[flipPos - 1], path[flipPos]);
                        }
                }
};

#endif  // DELTA_ORIENTATIONS_LOCALSEARCHRWALK_H

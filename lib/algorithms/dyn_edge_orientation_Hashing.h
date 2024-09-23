#ifndef DELTA_ORIENTATIONS_HASHING_H
#define DELTA_ORIENTATIONS_HASHING_H

#include <memory>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"
#include "priority_queues/bucket_pq.h"

class dyn_edge_orientation_Hashing : public dyn_edge_orientation {
        public:
                dyn_edge_orientation_Hashing(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config, DeltaOrientationsResult& result);

                void handleInsertion(NodeID source, NodeID target) override;
                void handleDeletion(NodeID source, NodeID target) override;
                void end() {
                };

                bool adjacent(NodeID source, NodeID target) {
                        return m_adj[source].find(target) != m_adj[source].end();
                }
        private:
                std::vector< std::unordered_map<NodeID, bool> > m_adj;
};



#endif  

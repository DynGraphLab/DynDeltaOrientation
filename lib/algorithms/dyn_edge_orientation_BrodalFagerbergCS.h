#ifndef DELTA_ORIENTATIONS_BrodalCS_H
#define DELTA_ORIENTATIONS_BrodalCS_H

#include <memory>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"

class dyn_edge_orientation_BrodalFagerbergCS : public dyn_edge_orientation {
        public:
                dyn_edge_orientation_BrodalFagerbergCS(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                                DeltaOrientationsResult& result);

                void handleInsertion(NodeID source, NodeID target) override;
                void handleDeletion(NodeID source, NodeID target) override;
                void end() {
                        for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                                for( unsigned j = 0; j < m_adj[i].size(); j++) {
                                        GOrientation->new_edge(i, m_adj[i][j]);
                                }
                        }
                };

                bool adjacent(NodeID source, NodeID target) {
                        for( unsigned i = 0; i < m_adj[source].size(); i++) {
                                if( m_adj[source][i] == target ) return true;
                        }
                        for( unsigned i = 0; i < m_adj[target].size(); i++) {
                                if( m_adj[target][i] == source ) return true;
                        }

                        return false;
                }
        private:

                void rebuild();
                void brodalfagerberg(NodeID source);

                std::vector< std::vector<NodeID> > m_adj;
                std::vector< std::pair< bool, std::pair< NodeID, NodeID > > > m_sequence;
                int m_bound_arboricity;
                long m_reorientations_limit;
                long m_cur_reorientations;
                int m_rebuild_count = 0;
                bool m_rebuild;
};



#endif  

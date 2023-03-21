//
//

#include <cmath>
#include "dyn_edge_orientation_Naive.h"
#include "random_functions.h"

dyn_edge_orientation_Naive::dyn_edge_orientation_Naive(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result) {
                m_adj.resize(GOrientation->number_of_nodes()); 
        }

void dyn_edge_orientation_Naive::handleInsertion(NodeID source, NodeID target) {
        if( m_adj[source].size() < m_adj[target].size() ){
                m_adj[source].push_back(target);
        } else {
                m_adj[target].push_back(source);
        }
}

void dyn_edge_orientation_Naive::handleDeletion(NodeID source, NodeID target) {
        for( unsigned i = 0; i < m_adj[source].size(); i++) {
                if( m_adj[source][i] == target ) {
                        std::swap(m_adj[source][i], m_adj[source][m_adj[source].size()-1]);
                        m_adj[source].pop_back();
                        break;
                }
        }
        for( unsigned i = 0; i < m_adj[target].size(); i++) {
                if( m_adj[target][i] == source ) {
                        std::swap(m_adj[target][i], m_adj[target][m_adj[target].size()-1]);
                        m_adj[target].pop_back();
                        break;
                }
        }

}



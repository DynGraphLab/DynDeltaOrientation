//
//

#include <cmath>
#include "dyn_edge_orientation_HashingGoogle.h"
#include "random_functions.h"

dyn_edge_orientation_HashingGoogle::dyn_edge_orientation_HashingGoogle(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result) {
                m_adj.resize(GOrientation->number_of_nodes()); 
                for( unsigned i = 0; i < m_adj.size(); i++) {
                                m_adj[i].set_deleted_key(std::numeric_limits<NodeID>::max());
                                m_adj[i].set_empty_key(std::numeric_limits<NodeID>::max()-1);
                                m_adj[i].min_load_factor(0.0);
                }
        }

void dyn_edge_orientation_HashingGoogle::handleInsertion(NodeID source, NodeID target) {
        m_adj[source][target] = true;
        m_adj[target][source] = true;
}

void dyn_edge_orientation_HashingGoogle::handleDeletion(NodeID source, NodeID target) {
        m_adj[source].erase(m_adj[source].find(target));
        m_adj[target].erase(m_adj[target].find(source));

}



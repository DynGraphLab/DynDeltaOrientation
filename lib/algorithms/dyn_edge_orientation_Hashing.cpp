//
//

#include <cmath>
#include "dyn_edge_orientation_Hashing.h"
#include "random_functions.h"

dyn_edge_orientation_Hashing::dyn_edge_orientation_Hashing(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result) {
                m_adj.resize(GOrientation->number_of_nodes()); 
        }

void dyn_edge_orientation_Hashing::handleInsertion(NodeID source, NodeID target) {
        m_adj[source][target] = true;
        m_adj[target][source] = true;
}

void dyn_edge_orientation_Hashing::handleDeletion(NodeID source, NodeID target) {
        m_adj[source].erase(m_adj[source].find(target));
        m_adj[target].erase(m_adj[target].find(source));

}



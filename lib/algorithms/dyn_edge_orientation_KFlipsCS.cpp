//
//

#include <cmath>
#include "dyn_edge_orientation_KFlipsCS.h"
#include "random_functions.h"

dyn_edge_orientation_KFlipsCS::dyn_edge_orientation_KFlipsCS(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result) {
                m_degree.resize(GOrientation->number_of_nodes(), 0); 
                m_touched.resize(GOrientation->number_of_nodes(), false); 
                m_bucket_pos.resize(GOrientation->number_of_nodes(),0); 
                m_adj.resize(GOrientation->number_of_nodes()); 
                m_degree_buckets.resize(GOrientation->number_of_nodes()); 
                m_max_degree = 0;

                for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                        m_degree_buckets[0].push_back(i);
                        m_bucket_pos[i] = i; // store the bucket position of node i
                }
        }

void dyn_edge_orientation_KFlipsCS::handleInsertion(NodeID source, NodeID target) {
        m_adj[source].push_back(target);
        move_node_to_new_bucket(source, m_degree[source]+1);

        m_degree[source]++;

        int old_max = m_max_degree;
        if( m_max_degree==1 || (config.pruneFlips && m_degree[source] < m_max_degree )) {
                return;
        } else {
                kflips();
        }
}

void dyn_edge_orientation_KFlipsCS::handleDeletion(NodeID source, NodeID target) {
        for( unsigned i = 0; i < m_adj[source].size(); i++) {
                if( m_adj[source][i] == target ) {
                        std::swap(m_adj[source][i], m_adj[source][m_adj[source].size()-1]);
                        m_adj[source].pop_back();
                        move_node_to_new_bucket(source, m_degree[source]-1);
                        m_degree[source]--;

                        break;
                }
        }
        for( unsigned i = 0; i < m_adj[target].size(); i++) {
                if( m_adj[target][i] == source ) {
                        std::swap(m_adj[target][i], m_adj[target][m_adj[target].size()-1]);
                        m_adj[target].pop_back();
                        move_node_to_new_bucket(target, m_degree[target]-1);
                        m_degree[target]--;

                        break;
                }
        }

        kflips();
}

void dyn_edge_orientation_KFlipsCS::kflips() {
        for( unsigned i = 0; i < config.flips; i++) {
                NodeID source = m_degree_buckets[m_max_degree][random_functions::nextInt(0, m_degree_buckets[m_max_degree].size()-1)];
                NodeID target = m_adj[source].front();

                m_adj[source].pop_front();
                m_adj[target].push_back(source);

                move_node_to_new_bucket(source, m_degree[source]-1);
                m_degree[source]--;

                move_node_to_new_bucket(target, m_degree[target]+1);
                m_degree[target]++;

                if( m_max_degree == 1 ) break; // we reached opt
        }
}

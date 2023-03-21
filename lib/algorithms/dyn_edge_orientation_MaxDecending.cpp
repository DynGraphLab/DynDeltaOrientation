//
//

#include <cmath>
#include "dyn_edge_orientation_MaxDecending.h"
#include "random_functions.h"

dyn_edge_orientation_MaxDecending::dyn_edge_orientation_MaxDecending(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result) {
                m_degree.resize(GOrientation->number_of_nodes(), 0); 
                m_bucket_pos.resize(GOrientation->number_of_nodes(),0); 
                m_adj.resize(GOrientation->number_of_nodes()); 
                m_degree_buckets.resize(GOrientation->number_of_nodes()); 
                m_max_degree = 0;

                for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                        m_degree_buckets[0].push_back(i);
                        m_bucket_pos[i] = i; // store the bucket position of node i
                }
        }

void dyn_edge_orientation_MaxDecending::handleInsertion(NodeID source, NodeID target) {
        m_adj[source].push_back(target);
        move_node_to_new_bucket(source, m_degree[source]+1);

        m_degree[source]++;

        int old_max = m_max_degree;
        if( m_max_degree==1 || (config.pruneFlips && m_degree[source] < m_max_degree )) {
                return;
        } else {
                int depth = findsequence(source, 0);
                while(depth) {
                        depth = findsequence(source, 0);
                };
        }
}

void dyn_edge_orientation_MaxDecending::handleDeletion(NodeID source, NodeID target) {
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

}

int dyn_edge_orientation_MaxDecending::findsequence(NodeID source, int depth) {
        // first find node with smallest outdegree in neighborhood
        int mindegree = m_degree[source]-1;
        int minnode = -1;
        int minnodeidx = -1;
        for( unsigned i = 0; i < m_adj[source].size(); i++) {
                NodeID curNode = m_adj[source][i];
                if( m_degree[curNode] < mindegree ) {
                        mindegree = m_degree[curNode];
                        minnode = curNode;
                        minnodeidx = i;
                }
        }

        if( minnode != -1 ) {
                // swap with that node
                std::swap(m_adj[source][minnodeidx], m_adj[source][m_adj[source].size()-1]);
                m_adj[source].pop_back();

                m_adj[minnode].push_back(source);
                
                move_node_to_new_bucket(source, m_degree[source]-1);
                m_degree[source]--;

                move_node_to_new_bucket(minnode, m_degree[minnode]+1);
                m_degree[minnode]++;

                return findsequence(minnode, depth+1);
        }

        return depth;
}

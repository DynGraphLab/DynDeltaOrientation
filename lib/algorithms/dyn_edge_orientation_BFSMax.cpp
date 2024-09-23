//
//

#include <cmath>
#include "dyn_edge_orientation_BFSMax.h"
#include "random_functions.h"

dyn_edge_orientation_BFSMax::dyn_edge_orientation_BFSMax(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result) {
                m_degree.resize(GOrientation->number_of_nodes(), 0); 
                m_parent.resize(GOrientation->number_of_nodes(), 0); 
                m_depth.resize(GOrientation->number_of_nodes(), -1); 
                m_bucket_pos.resize(GOrientation->number_of_nodes(),0); 
                m_adj.resize(GOrientation->number_of_nodes()); 
                m_degree_buckets.resize(GOrientation->number_of_nodes()); 
                m_max_degree = 0;

                for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                        m_degree_buckets[0].push_back(i);
                        m_bucket_pos[i] = i; // store the bucket position of node i
                }
        }

void dyn_edge_orientation_BFSMax::handleInsertion(NodeID source, NodeID target) {
        m_adj[source].push_back(target);
        int old_max = m_max_degree;
        move_node_to_new_bucket(source, m_degree[source]+1);

        m_degree[source]++;

        if( m_degree[source] < m_max_degree || m_max_degree == 1) {
                return;
        } else {
                int max_iterations = std::min((int)config.bfsMaxAlgoAmount, (int)(1.5*m_degree_buckets[m_max_degree].size()));
                for( unsigned i = 0; i < max_iterations; i++) {
                        NodeID startNode = m_degree_buckets[m_max_degree][random_functions::nextInt(0, m_degree_buckets[m_max_degree].size()-1)];
                        bfs(startNode);
                        if( m_max_degree == 1 ) break; // we reached opt
                }
        }
}

void dyn_edge_orientation_BFSMax::handleDeletion(NodeID source, NodeID target) {
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

void dyn_edge_orientation_BFSMax::bfs(NodeID source) {
        std::vector< NodeID > touched;
        std::queue< NodeID > bfsqueue;
        touched.push_back(source);
        bfsqueue.push(source);
        m_parent[source]  = source;
        m_depth[source]  = 0;
        int cur_depth  = 0;
        bool found_lower_deg_vertex = false;
        NodeID final_vertex;

        while(!bfsqueue.empty()) {
                NodeID node = bfsqueue.front();
                bfsqueue.pop(); 

                if( m_degree[node] < m_max_degree - 1 ) {
                        found_lower_deg_vertex = true;
                        final_vertex = node;
                        break;
                }

                if( cur_depth > config.bfsDepthBound ) {
                        break;
                }

                if (m_depth[node] == cur_depth) {
                        cur_depth++;
                }

                for( unsigned i = 0; i < m_adj[node].size(); i++) {
                        NodeID target = m_adj[node][i];
                        if(m_depth[target] == -1) {
                                m_depth[target] = cur_depth;
                                bfsqueue.push(target);
                                m_parent[target] = node;
                                touched.push_back(target);
                        }
                }
        }

        if( found_lower_deg_vertex ) {
                // follow path and swap it
                NodeID curNode = final_vertex;
                move_node_to_new_bucket(final_vertex, m_degree[final_vertex]+1);
                m_degree[final_vertex]++;

                while ( m_parent[curNode ] != curNode ) {
                        NodeID parent = m_parent[curNode];
                        // insert parent in current nodes adjacency
                        m_adj[curNode].push_back(parent);
                        // remove curNode from parents adjacency 
                        // node finding the vertex in the adjaceny does not increase complexity as we did a bfs before and checked all neighbors
                        for( unsigned i = 0; i < m_adj[parent].size(); i++) {
                                if(m_adj[parent][i] == curNode) {
                                        std::swap(m_adj[parent][i], m_adj[parent][m_adj[parent].size()-1]);
                                        m_adj[parent].pop_back();
                                        break;
                                }
                        }
                        curNode = m_parent[curNode];
                }; 
                move_node_to_new_bucket(curNode, m_degree[curNode]-1);
                m_degree[curNode]--;
        }

        for( NodeID v : touched ) {
                m_parent[v] = 0;
                m_depth[v]  = -1;
        }
}



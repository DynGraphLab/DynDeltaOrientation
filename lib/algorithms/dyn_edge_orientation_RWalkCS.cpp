//
//

#include <cmath>
#include "dyn_edge_orientation_RWalkCS.h"
#include "random_functions.h"

dyn_edge_orientation_RWalkCS::dyn_edge_orientation_RWalkCS(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
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

void dyn_edge_orientation_RWalkCS::handleInsertion(NodeID source, NodeID target) {
        m_adj[source].push_back(target);
        move_node_to_new_bucket(source, m_degree[source]+1);

        m_degree[source]++;

        int old_max = m_max_degree;
        if( m_degree[source] < m_max_degree || m_max_degree==1) {
                return;
        } else {
                for( unsigned i = 0; i < config.rwAmount; i++) {
                        if(rwalk(source)) break;
                }
        }
}

void dyn_edge_orientation_RWalkCS::handleDeletion(NodeID source, NodeID target) {
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

bool dyn_edge_orientation_RWalkCS::rwalk(NodeID source) {
        NodeID curNode = source;

        std::vector< NodeID > undo;
        undo.push_back(source);
        m_touched[source] = true;

        std::vector< NodeID > path;
        path.push_back(source);

        std::vector< NodeID > idx;
        idx.push_back(0);

        for( unsigned i = 0; i < config.rwLengthBound; i++) {
                NodeID rndNeighborID = random_functions::nextInt(0, m_adj[curNode].size()-1);
                NodeID rndNeighbor = m_adj[curNode][rndNeighborID];
                int cnt = 0;
                while ( m_touched[ rndNeighbor ] &&  cnt < 3 ) {
                        rndNeighborID = random_functions::nextInt(0, m_adj[curNode].size()-1);
                        rndNeighbor = m_adj[curNode][rndNeighborID];
                        cnt++;
                }

                path.push_back(rndNeighbor);
                idx.push_back(rndNeighborID);

                curNode = rndNeighbor;

                if( m_touched[ rndNeighbor ] ) {
                        break;
                }

                m_touched[curNode] = true;
                undo.push_back(curNode);

                if( m_adj[curNode].size() < m_max_degree-1 ) {
                        break;
                }
        }

       bool success = false;
       if( m_adj[curNode].size() < m_max_degree-1 ) {
               success = true;
               // do the acutal changes
               for( unsigned i = 0; i < path.size()-1; i++) {
                       NodeID curNode = path[i];
                       NodeID nextNode = path[i+1];

                       // remove nextNode from curNode 
                       std::swap(m_adj[curNode][idx[i+1]], m_adj[curNode][m_adj[curNode].size()-1]);
                       m_adj[curNode].pop_back();                
                       m_adj[nextNode].push_back(curNode);
               }
               move_node_to_new_bucket(source, m_degree[source]-1);
               m_degree[source]--;

               NodeID lastNode = path.back();
               move_node_to_new_bucket(lastNode, m_degree[lastNode]+1);
               m_degree[lastNode]++;
       }

       for( unsigned i = 0; i < undo.size(); i++) {
               m_touched[undo[i]] = false;
       }

       return success;
}



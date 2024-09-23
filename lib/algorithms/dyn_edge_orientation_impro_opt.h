#ifndef DELTA_ORIENTATIONS_impro_opt_H
#define DELTA_ORIENTATIONS_impro_opt_H

#include <memory>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"
#include "priority_queues/bucket_pq.h"

class dyn_edge_orientation_impro_opt : public dyn_edge_orientation {
        public:
                dyn_edge_orientation_impro_opt(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                                DeltaOrientationsResult& result);

                void handleInsertion(NodeID source, NodeID target) override;
                void handleDeletion(NodeID source, NodeID target) override;
                void end() {
                        checkdegrees();
                        for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                                for( unsigned j = 0; j < m_adj[i].size(); j++) {
                                        GOrientation->new_edge(i, m_adj[i][j]);
                                }
                        }
                        std::cout <<  "internal max deg " <<  m_max_degree  << std::endl;
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

                void bfs(NodeID source);
                void checkdegrees() {
                        int max = 0;
                        for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                                if( m_degree[i] != m_adj[i].size() ) {
                                        std::cout <<  "problem with degrees"  << std::endl;
                                        exit(0);
                                }
                                if( m_adj[i].size() > max ) {
                                        max = m_adj[i].size();
                                }
                        }
                        for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                                if(  m_degree_buckets[m_degree[i]].size() < m_bucket_pos[i] ) {
                                        std::cout <<  "size problem " <<  m_bucket_pos[i] << " " << m_degree_buckets[m_degree[i]].size()<< std::endl;
                                        exit(0);
                                }
                                if( m_degree_buckets[m_degree[i]][m_bucket_pos[i]] != i  ) {
                                        std::cout <<  "bucket pos error"  << std::endl;
                                        exit(0);
                                }
                        }
                        if( max != m_max_degree ) {
                                std::cout <<  "max degree does not match"  << std::endl;
                                exit(0);
                        }

                };

                void move_node_to_new_bucket(NodeID source, int new_bucket) {
                        // move vertex out of its current bucket into the larger bucket
                        int bucket_pos = m_bucket_pos[source];
                        int bucket_size = m_degree_buckets[m_degree[source]].size();

                        std::swap(m_degree_buckets[m_degree[source]][bucket_pos], m_degree_buckets[m_degree[source]][bucket_size-1]);
                        m_degree_buckets[m_degree[source]].pop_back(); // source is now removed from the bucket
                        m_bucket_pos[m_degree_buckets[m_degree[source]][bucket_pos]] = bucket_pos;
                        m_degree_buckets[new_bucket].push_back(source);
                        m_bucket_pos[source] = m_degree_buckets[new_bucket].size() -1;

                        if( new_bucket > m_max_degree ) {
                                m_max_degree = new_bucket;
                        }

                        // did we decrease the max degree?? 
                        while( m_degree_buckets[m_max_degree].size() == 0) m_max_degree--;
                };

                std::vector<int> m_parent;
                std::vector<NodeID> m_depth;
                std::vector< std::vector<NodeID> > m_degree_buckets;
                std::vector<NodeID> m_bucket_pos;
                std::vector<int> m_degree;
                std::vector< std::vector<NodeID> > m_adj;
                int m_max_degree;
};



#endif  

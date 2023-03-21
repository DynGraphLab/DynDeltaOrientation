//
//

#include <cmath>
#include <stack>
#include "dyn_edge_orientation_BrodalFagerbergCS.h"
#include "random_functions.h"

dyn_edge_orientation_BrodalFagerbergCS::dyn_edge_orientation_BrodalFagerbergCS(const std::shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                DeltaOrientationsResult& result)
        : dyn_edge_orientation(GOrientation, config, result) {
                m_bound_arboricity = 1;
                m_reorientations_limit = 0;
                m_cur_reorientations = 0;
                m_adj.resize(GOrientation->number_of_nodes()); 
                m_rebuild = false;
}

void dyn_edge_orientation_BrodalFagerbergCS::handleInsertion(NodeID source, NodeID target) {
        m_adj[source].push_back(target);
        if(!m_rebuild) {
                m_sequence.push_back( std::make_pair(true, std::make_pair(source, target)));
        }
        m_reorientations_limit += m_bound_arboricity + 1;

        brodalfagerberg(source);
}

void dyn_edge_orientation_BrodalFagerbergCS::handleDeletion(NodeID source, NodeID target) {
        if(!m_rebuild) m_sequence.push_back( std::make_pair(false, std::make_pair(source, target)));
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

void dyn_edge_orientation_BrodalFagerbergCS::rebuild() {
        m_rebuild = true;

        for( unsigned i = 0; i < m_adj.size(); i++) {
                m_adj[i].clear();
        }

        m_bound_arboricity = ceil(config.bfgrowthfactor*m_bound_arboricity);
        m_cur_reorientations = 0;
        m_reorientations_limit = m_bound_arboricity + 1;

        for( unsigned i = 0; i < m_sequence.size(); i++) {
                if( m_sequence[i].first ) {
                        handleInsertion(m_sequence[i].second.first, m_sequence[i].second.second);
                } else {
                        handleDeletion(m_sequence[i].second.first, m_sequence[i].second.second);
                }
                if(m_cur_reorientations >= m_reorientations_limit ) {
                        break;
                }

        }
        std::cout <<  "rebuild, new bound DELTA " << m_bound_arboricity  << std::endl;
        std::cout <<  "rebuild, new limit " << m_reorientations_limit << std::endl;
        if(m_cur_reorientations >= m_reorientations_limit ) {
                rebuild();
        }


        m_rebuild = false;
}


void dyn_edge_orientation_BrodalFagerbergCS::brodalfagerberg(NodeID source) {
        if( m_adj[source].size() == m_bound_arboricity + 1) {
                std::stack< NodeID > S;
                S.push(source);

                while(!S.empty()) {
                        NodeID w = S.top();
                        S.pop();

                        if( m_adj[w].size() >= m_bound_arboricity + 1 ) {
                                for( unsigned i = 0; i < m_adj[w].size(); i++, m_cur_reorientations++) {
                                        NodeID x = m_adj[w][i];
                                        m_adj[x].push_back(w);
                                        if( m_adj[x].size() == m_bound_arboricity + 1 ) {
                                                S.push(x);
                                        }

                                        if(m_cur_reorientations >= m_reorientations_limit ) {
                                                if(!m_rebuild) {
                                                        return rebuild();
                                                } else {return;}
                                        }
                                }
                                m_adj[w].clear();
                        }

                }
        
        }
}



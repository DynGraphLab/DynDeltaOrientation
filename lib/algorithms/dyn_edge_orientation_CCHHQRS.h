#ifndef DELTA_ORIENTATIONS_CCHHQRS
#define DELTA_ORIENTATIONS_CCHHQRS

#include <memory>
#include <bits/stdc++.h>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"
#include "buckets.h"

class dyn_edge_orientation_CCHHQRS : public dyn_edge_orientation {
        public:
                dyn_edge_orientation_CCHHQRS(const shared_ptr<dyn_graph_access>& GOrientation, const DeltaOrientationsConfig& config,
                                DeltaOrientationsResult& result);
                void handleInsertion(NodeID source, NodeID target) override;
                void handleDeletion(NodeID source, NodeID target) override;
                void end() override {
                        for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                                for (DEdge* uv: vertices[i].out_edges){
                                        // "complete" edges
                                        for (int j = 0; j < uv->count / config.b; j++){
                                                m_adj[i].push_back(uv->target->node);
                                        }
                                        // "partial" edges
                                        // if there is a half edge, we break ties arbitrarily
                                        if (2 * (uv->count % config.b) == config.b){
                                                if (i > uv->target->node){ m_adj[i].push_back(uv->target->node); }
                                        }
                                        else if (uv->count % config.b > config.b / 2){ m_adj[i].push_back(uv->target->node); }
                                }
                        }
                        for( unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                                for( unsigned j = 0; j < m_adj[i].size(); j++) {
                                        GOrientation->new_edge(i, m_adj[i][j]);
                                }
                        }
                };

                bool adjacent(NodeID source, NodeID target) override {
                        for(unsigned int i : m_adj[source]) {
                                if( i == target ) return true;
                        }
                        for(unsigned int i : m_adj[target]) {
                                if( i == source ) return true;
                        }
                        return false;
                }
        private:
                vector< vector<NodeID> > m_adj;                 // For post processing only
                map<pair<int, int>, DEdge> edges;
                vector<Vertex> vertices;
                void insert_directed(DEdge *uv);
                void delete_directed(DEdge *uv);
                void add(DEdge *uv);
                void add_fast(DEdge *uv, list<pair<NodeID, int>>::iterator uv_iterator);
                void remove(DEdge *uv);
                list<pair<NodeID, int>>::iterator argmin_out(NodeID source);
};

#endif

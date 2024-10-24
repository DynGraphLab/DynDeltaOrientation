#ifndef DELTA_ORIENTATIONS_CCHHQRS
#define DELTA_ORIENTATIONS_CCHHQRS

#include <bits/stdc++.h>
#include <memory>
#include <vector>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "buckets.h"
#include "dyn_graph_access.h"

class dyn_edge_orientation_CCHHQRS : public dyn_edge_orientation {
public:
        dyn_edge_orientation_CCHHQRS(
                const std::shared_ptr<dyn_graph_access>& GOrientation,
                const DeltaOrientationsConfig& config, DeltaOrientationsResult& result);
        void handleInsertion(NodeID source, NodeID target) override;
        void handleDeletion(NodeID source, NodeID target) override;

        void end() override {
                for (unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                        for (DEdge* uv : vertices[i].out_edges) {
                                // "complete" edges
                                for (int j = 0; j < uv->count / config.b; j++) { m_adj[i].push_back(uv->target); }
                                // "partial" edges
                                // if there is a half edge, we break ties arbitrarily
                                if (2 * (uv->count % config.b) == config.b) {
                                        if (i > uv->target) { m_adj[i].push_back(uv->target); }
                                }
                                else if (uv->count % config.b > config.b / 2) { m_adj[i].push_back(uv->target); }
                        }
                }
                for (unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
                        for (unsigned j = 0; j < m_adj[i].size(); j++) { GOrientation->new_edge(i, m_adj[i][j]); }
                }
        };

        bool adjacent(NodeID source, NodeID target) override {
                for (unsigned int i : m_adj[source]) {
                        if (i == target)
                                return true;
                }
                for (unsigned int i : m_adj[target]) {
                        if (i == source)
                                return true;
                }
                return false;
        }

private:
        std::vector<std::vector<NodeID>> m_adj; // For post processing only
        std::vector<DEdge*> edge_allocator; // TODO delete edges at the end
        std::vector<Vertex> vertices;
        void insert_directed(DEdge* uv, NodeID u);
        void delete_directed(DEdge* uv, NodeID u);
        void add(DEdge* uv, NodeID u);
        // void add_fast(DEdge *uv, list<pair<NodeID, int>>::iterator uv_iterator);
        void remove(DEdge* uv, NodeID v);
        // list<pair<NodeID, int>>::iterator argmin_out(NodeID source);
};

#endif

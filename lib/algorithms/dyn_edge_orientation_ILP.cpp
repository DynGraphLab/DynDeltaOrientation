//
//
#ifdef ILP
#include "dyn_edge_orientation_ILP.h"
#include "ilp.h"

void dyn_edge_orientation_ILP::handleInsertion(NodeID source, NodeID target) {
        // we only solve the ILP in the end
        GOrientation->new_edge(target, source); // maintain undirected graph
}

void dyn_edge_orientation_ILP::handleDeletion(NodeID source, NodeID target) {
        // we only solve the ILP in the end
        //
        GOrientation->remove_edge(target, source); // maintain undirected graph
        GOrientation->remove_edge(source, target); // maintain undirected graph
}

void dyn_edge_orientation_ILP::solveILP() {

        // init ILP
        std::vector< std::pair< int, int > > edges;
        // build undirected graph from orientation
        forall_nodes((*GOrientation), node) {
                forall_out_edges((*GOrientation), e, node) {
                        NodeID target = GOrientation->getEdgeTarget(node, e);

                        std::pair<int, int> p;
                        p.first = node; 
                        p.second = target;
                        edges.push_back(p);
                } endfor
        } endfor

        graph_access G; 
        if(config.relaxILP) {
                solveUsingRelaxedILP(G, *GOrientation, config);
        }else{
                solveUsingILP(G, *GOrientation, config);
        }

        for( unsigned i = 0; i < edges.size(); i++) {
                GOrientation->remove_edge(edges[i].first, edges[i].second);
        }

        forall_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        GOrientation->new_edge(node,target);
                } endfor
        } endfor
        
}
#endif

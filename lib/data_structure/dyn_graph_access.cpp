/******************************************************************************
 * dyn_graph_access.cpp
 *
 *
 *****************************************************************************/

#include "dyn_graph_access.h"

dyn_graph_access::dyn_graph_access() : m_building_graph(false) {
        edge_count = 0;
        node_count = 0;
        //node_capacity = 0;
}

dyn_graph_access::dyn_graph_access(NodeID n) : m_building_graph(false) {
        edge_count = 0;
        node_count = n;
        start_construction(node_count, edge_count);
        finish_construction();
}

dyn_graph_access::~dyn_graph_access() = default;

void dyn_graph_access::start_construction(NodeID nodes, EdgeID edges) {
        m_building_graph = true;
        m_edges.resize(nodes);
        m_edges_maps.resize(nodes);
        m_rev_edges_maps.resize(nodes);
        m_nodes.resize(nodes);

        for( unsigned i = 0; i < nodes; i++) {
                m_edges_maps[i].set_deleted_key(std::numeric_limits<NodeID>::max());
                m_edges_maps[i].set_empty_key(std::numeric_limits<NodeID>::max()-1);
                m_edges_maps[i].min_load_factor(0.0);
                m_rev_edges_maps[i].set_deleted_key(std::numeric_limits<NodeID>::max());
                m_rev_edges_maps[i].set_empty_key(std::numeric_limits<NodeID>::max()-1);
                m_rev_edges_maps[i].min_load_factor(0.0);
        }
}

NodeID dyn_graph_access::new_node() {
        ASSERT_TRUE(m_building_graph);
        return node_count++;
}

void dyn_graph_access::finish_construction() {
        m_building_graph = false;
        m_edges.resize(node_count);
        m_edges_maps.resize(node_count);
        m_rev_edges_maps.resize(node_count);
}

void dyn_graph_access::convert_from_graph_access(graph_access& H) {
        std::cout <<  "TODO"  << std::endl;
        exit(0);
        //m_building_graph = false;
        //edge_count = 0;
        //node_count = H.number_of_nodes();
        //start_construction(node_count, edge_count);

        //forall_nodes(H,n)
                //forall_out_edges(H,e,n)
                        //NodeID target = H.getEdgeTarget(e);
                        //new_edge(n,target);
                //endfor
        //endfor

        //ASSERT_TRUE(number_of_nodes() == H.number_of_nodes());
        //ASSERT_TRUE(number_of_edges() == H.number_of_edges());
}

void dyn_graph_access::convert_to_graph_access(graph_access& H) {

        std::vector< std::vector< NodeID > > edges(number_of_nodes());

        //we are an undirected graph so we need to replicated edges to their counter parts
        forall_nodes((*this), node) {
                forall_out_edges((*this), e, node) {
                        NodeID target = (*this).getEdgeTarget(node, e);
                        edges[node].push_back(target);
                        edges[target].push_back(node);
                } endfor
        } endfor
        

        H.start_construction(number_of_nodes(), 2*number_of_edges());

        forall_nodes((*this), node) {
                H.new_node();
                for( unsigned i = 0; i < edges[node].size(); i++) {
                        H.new_edge(node, edges[node][i]);
                }
        } endfor

        H.finish_construction();
}








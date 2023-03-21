/******************************************************************************
 *
 *****************************************************************************/

#ifndef dyn_graph_access_EFRXO4X2
#define dyn_graph_access_EFRXO4X2

#include "graph_access.h"

#include <bitset>
#include <cassert>
#include <iostream>
#include <sparsehash/dense_hash_map>
#include <vector>

#include "../definitions.h"
#include "macros_assertions.h"
//#include "IGraph.h"

class dyn_graph_access {
   public:
    dyn_graph_access();
    explicit dyn_graph_access(NodeID n);
    ~dyn_graph_access();

    // construction of the graph
    void start_construction(NodeID nodes, EdgeID edges);
    NodeID new_node();
    void finish_construction();

    void convert_from_graph_access(graph_access& H);
    void convert_to_graph_access(graph_access& H);

    inline bool new_edge(NodeID source, NodeID target);
    inline bool remove_edge(NodeID source, NodeID target);

    inline bool isEdge(NodeID source, NodeID target);

    // access
    inline EdgeID number_of_edges();
    inline NodeID number_of_nodes();

    inline EdgeID get_first_edge(NodeID node);
    inline EdgeID get_first_invalid_edge(NodeID node);

    inline EdgeID getNodeDegree(NodeID node);
    inline NodeID getEdgeTarget(NodeID source, EdgeID edge);

    EdgeID maxDegree() {
        EdgeID currentMax = 0;
        for (NodeID node = 0; node < node_count; ++node) {
                EdgeID deg = getNodeDegree(node);
                if (deg > currentMax) currentMax = deg;
        }
        return currentMax;
    }

   private:
    // %%%%%%%%%%%%%%%%%%% DATA %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    std::vector<std::vector<DynEdge> > m_edges;
    std::vector<google::dense_hash_map<NodeID, size_t> > m_edges_maps;
    std::vector<google::dense_hash_map<NodeID, size_t> > m_rev_edges_maps;

    std::vector<DynNode> m_nodes;

    EdgeID edge_count;
    NodeID node_count;

    // construction properties
    bool m_building_graph;
};

bool dyn_graph_access::new_edge(NodeID source, NodeID target) {
    ASSERT_TRUE(&(m_edges_maps[source]) != 0);
    //ASSERT_TRUE(target < node_capacity);
    //ASSERT_TRUE(source < node_capacity);

    // look up if edge exists. if not, continue
    if (m_edges_maps[source].find(target) == m_edges_maps[source].end()) {
        // create new edge with target node to add to the end of
        // the corresponding edges-vector
        DynEdge e_buf;
        e_buf.target = target;
        m_edges[source].push_back(e_buf);

        // add the index of the new edge and the target node as
        // <key,value>-pair to the unordered map
        m_edges_maps[source][target] = m_edges[source].size() - 1;
        m_rev_edges_maps[target][source] = m_edges[source].size() - 1;

        edge_count++;
        return true;
    } else {
        return false;
    }
}

bool dyn_graph_access::remove_edge(NodeID source, NodeID target) {
    // look up edge before removing
    if (m_edges_maps[source].find(target) != m_edges_maps[source].end()) {
        size_t i = m_edges_maps[source][target];
        // copy the last element and paste it to the position of the edge which is
        // supposed to be deleted. then truncate the corresponding vector
        DynEdge e_buf = m_edges[source].back();
        m_edges[source][i] = e_buf;
        m_edges[source].pop_back();

        // adjust the entries in the unordered map since the index of the last
        // edge has now changed to the index of the edge which has been deleted.
        m_edges_maps[source][e_buf.target] = i;
        m_edges_maps[source].erase(target);

        m_rev_edges_maps[e_buf.target][source] = i;
        m_rev_edges_maps[target].erase(source);

        edge_count--;
        return true;
    } else {
        return false;
    }
}

bool dyn_graph_access::isEdge(NodeID source, NodeID target) {
    return (m_edges_maps[source].find(target) != m_edges_maps[source].end());
}

EdgeID dyn_graph_access::number_of_edges() { return edge_count; }

NodeID dyn_graph_access::number_of_nodes() { return node_count; }

//NodeID dyn_graph_access::capacity_of_nodes() { return node_capacity; }

EdgeID dyn_graph_access::get_first_edge(NodeID node) { return 0; }

EdgeID dyn_graph_access::get_first_invalid_edge(NodeID node) { return m_edges[node].size(); }

EdgeID dyn_graph_access::getNodeDegree(NodeID node) { return m_edges[node].size(); }

NodeID dyn_graph_access::getEdgeTarget(NodeID source, EdgeID edge) {
    ASSERT_TRUE(m_edges.size() > 0);
    ASSERT_TRUE(m_edges[source].size() > edge);

    return m_edges[source][edge].target;
}

// TODO implement method
//bool dyn_graph_access::new_node(NodeID node) {
    ////ASSERT_TRUE(node_capacity > node);
    ////ASSERT_TRUE(m_nodes[node].deleted);

    ////m_nodes[node].deleted = false;
    //node_count++;

    //return true;
//}

//bool dyn_graph_access::remove_node(NodeID node) {
    //ASSERT_TRUE(node_count > node);
    ////ASSERT_TRUE(!m_nodes[node].deleted);

    ////m_nodes[node].deleted = true;
    ////m_nodes[node].weight = 1;

    //forall_out_edges ((*this), e, node)
        //remove_edge(node, getEdgeTarget(node, e));
    //endfor

    //for (auto it = m_rev_edges_maps[node].begin(); it != m_rev_edges_maps[node].end(); ++it) {
        //remove_edge(it->first, node);
    //}

    //node_count--;

    //return true;
//}

//bool dyn_graph_access::is_node(NodeID node) { return node < node_capacity && !m_nodes[node].deleted; }

#endif /* noend of include guard: dyn_graph_access_EFRXO4X2 */

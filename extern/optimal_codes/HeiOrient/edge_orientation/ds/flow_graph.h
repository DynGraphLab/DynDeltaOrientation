#pragma once

#include <vector>

namespace rpo::edge_orientation::ds {

template <typename NT, typename ET, typename FT = int>
class FlowGraph {
 public:
  using NodeType = NT;
  using EdgeType = ET;
  using FlowType = FT;
  struct rEdge {
    NodeType source;
    NodeType target;
    FlowType capacity;
    FlowType flow;
    EdgeType reverse_edge_index;

    rEdge(NodeType source, NodeType target, FlowType capacity, FlowType flow,
          EdgeType reverse_edge_index) {
      this->source = source;
      this->target = target;
      this->capacity = capacity;
      this->flow = flow;
      this->reverse_edge_index = reverse_edge_index;
    }
    rEdge() {}
  };
  FlowGraph() {
    m_num_edges = 0;
    m_num_nodes = 0;
  };

  virtual ~FlowGraph(){};

  void start_construction(NodeType nodes, EdgeType edges = 0) {
    m_adjacency_lists.resize(nodes);
    m_num_nodes = nodes;
    m_num_edges = edges;
  }

  void finish_construction(){};

  NodeType number_of_nodes() { return m_num_nodes; };
  EdgeType number_of_edges() { return m_num_edges; };

  NodeType getEdgeTarget(NodeType source, EdgeType e) {
    return m_adjacency_lists[source][e].target;
  }
  FlowType getEdgeCapacity(NodeType source, EdgeType e) {
    return m_adjacency_lists[source][e].capacity;
  }
  void setEdgeCapacity(NodeType source, EdgeType e, FlowType cap) {
    assert(cap >= 0);
    m_adjacency_lists[source][e].capacity = cap;
  }
  FlowType getEdgeFlow(NodeType source, EdgeType e) { return m_adjacency_lists[source][e].flow; }
  void setEdgeFlow(NodeType source, EdgeType e, FlowType flow) {
    //  assert(flow >= 0);
    m_adjacency_lists[source][e].flow = flow;
  }

  EdgeType getReverseEdge(NodeType source, EdgeType e) {
    return m_adjacency_lists[source][e].reverse_edge_index;
  }

  void new_edge(NodeType source, NodeType target, FlowType capacity) {
    m_adjacency_lists[source].push_back(
        rEdge(source, target, capacity, 0, m_adjacency_lists[target].size()));
    // for each edge we add a reverse edge
    m_adjacency_lists[target].push_back(
        rEdge(target, source, 0, 0, m_adjacency_lists[source].size() - 1));
    m_num_edges += 2;
  };

  EdgeType get_first_edge(NodeType node) { return 0; };
  EdgeType get_first_invalid_edge(NodeType node) { return m_adjacency_lists[node].size(); };

  std::vector<std::vector<rEdge> > m_adjacency_lists;
  NodeType m_num_nodes;
  EdgeType m_num_edges;
};
}  // namespace rpo::edge_orientation::ds
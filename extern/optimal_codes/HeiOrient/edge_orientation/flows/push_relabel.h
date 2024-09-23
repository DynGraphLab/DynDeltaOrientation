#pragma once

#include <queue>

#include "utils/logging.h"

namespace rpo::edge_orientation::flows {
namespace {
const long WORK_OP_RELABEL = 9;
const double GLOBAL_UPDATE_FRQ = 0.51;
const long WORK_NODE_TO_EDGES = 4;
}  // namespace
template <class flow_graph>
class push_relabel {
 public:
  using NodeType = typename flow_graph::NodeType;
  using EdgeType = typename flow_graph::EdgeType;
  using FlowType = typename flow_graph::FlowType;
  push_relabel() {}
  virtual ~push_relabel() {}

  void init(flow_graph& G, NodeType source, NodeType sink) {
    m_excess.resize(G.number_of_nodes(), 0);
    m_distance.resize(G.number_of_nodes(), 0);
    m_active.resize(G.number_of_nodes(), false);
    m_count.resize(2 * G.number_of_nodes(), 0);
    m_bfstouched.resize(G.number_of_nodes(), false);

    m_count[0] = G.number_of_nodes() - 1;
    m_count[G.number_of_nodes()] = 1;

    m_distance[source] = G.number_of_nodes();
    m_active[source] = true;
    m_active[sink] = true;

    for (auto e = G.get_first_edge(source); e < G.get_first_invalid_edge(source); e++) {
      m_excess[source] += G.getEdgeCapacity(source, e);
      push(source, e);
    }
  }

  // perform a backward bfs in the residual starting at the sink
  // to update distance labels
  void global_relabeling(NodeType source, NodeType sink) {
    std::queue<NodeType> Q;
    for (NodeType node = 0; node < m_G->number_of_nodes(); node++) {
      m_distance[node] = std::max(m_distance[node], m_G->number_of_nodes());
      m_bfstouched[node] = false;
    }

    Q.push(sink);
    m_bfstouched[sink] = true;
    m_bfstouched[source] = true;
    m_distance[sink] = 0;

    NodeType node = 0;
    while (!Q.empty()) {
      node = Q.front();
      Q.pop();

      for (auto e = m_G->get_first_edge(node); e < m_G->get_first_invalid_edge(node); e++) {
        NodeType target = m_G->getEdgeTarget(node, e);
        if (m_bfstouched[target]) continue;

        EdgeType rev_e = m_G->getReverseEdge(node, e);
        if (m_G->getEdgeCapacity(target, rev_e) > m_G->getEdgeFlow(target, rev_e)) {
          m_count[m_distance[target]]--;
          m_distance[target] = m_distance[node] + 1;
          m_count[m_distance[target]]++;
          Q.push(target);
          m_bfstouched[target] = true;
        }
      }
    }
  }

  // push flow from source to target if possible
  void push(NodeType source, EdgeType e) {
    m_pushes++;
    FlowType capacity = m_G->getEdgeCapacity(source, e);
    FlowType flow = m_G->getEdgeFlow(source, e);
    auto amount = std::min((long long)(capacity - flow), m_excess[source]);
    NodeType target = m_G->getEdgeTarget(source, e);

    if (m_distance[source] <= m_distance[target] || amount == 0) return;

    m_G->setEdgeFlow(source, e, flow + amount);

    EdgeType rev_e = m_G->getReverseEdge(source, e);
    FlowType rev_flow = m_G->getEdgeFlow(target, rev_e);
    m_G->setEdgeFlow(target, rev_e, rev_flow - amount);

    m_excess[source] -= amount;
    m_excess[target] += amount;

    enqueue(target);
  }

  // put a vertex in the FIFO queue of the algorithm
  void enqueue(NodeType target) {
    if (m_active[target]) return;
    if (m_excess[target] > 0) {
      m_active[target] = true;
      // m_Q.push(target, m_distance[target]);
      m_Q.push(target);
    }
  }
  auto tOf_S_T_Cut(flow_graph& G, std::vector<bool>& removed, NodeType source, NodeType target) {
    std::vector<NodeType> setT;
    std::vector<bool> visited(G.number_of_nodes());
    visited[target] = true;
    std::queue<NodeType> m_Q;
    m_Q.push(target);
    while (!m_Q.empty()) {
      auto v = m_Q.front();
      m_Q.pop();
      if (removed[v]) {
        continue;
      }
      if (v != target && v != source) {
        setT.push_back(v);
      }
      for (auto e = G.get_first_edge(v); e < G.get_first_invalid_edge(v); e++) {
        auto neighbor = G.getEdgeTarget(v, e);
        if (!visited[neighbor] && G.getEdgeFlow(neighbor, G.getReverseEdge(v, e)) <
                                      G.getEdgeCapacity(neighbor, G.getReverseEdge(v, e))) {
          m_Q.push(neighbor);
          visited[neighbor] = true;
        }
      }
    }

    return setT;
  }
  // try to push as much excess as possible out of the node node
  void discharge(NodeType node) {
    EdgeType end = m_G->get_first_invalid_edge(node);
    for (EdgeType e = m_G->get_first_edge(node); e < end && m_excess[node] > 0; ++e) {
      push(node, e);
    }

    if (m_excess[node] > 0) {
      if (m_count[m_distance[node]] == 1 && m_distance[node] < m_G->number_of_nodes()) {
        // hence this layer will be empty after the relabel step
        gap_heuristic(m_distance[node]);
      } else {
        relabel(node);
      }
    }
  }

  // gap heuristic
  void gap_heuristic(NodeType level) {
    m_gaps++;
    for (NodeType node = 0; node < m_G->number_of_nodes(); node++) {
      if (m_distance[node] < level) continue;
      m_count[m_distance[node]]--;
      m_distance[node] = std::max(m_distance[node], m_G->number_of_nodes());
      m_count[m_distance[node]]++;
      enqueue(node);
    }
  }

  // relabel a node with respect to its
  // neighboring nodes
  void relabel(NodeType node) {
    m_work += WORK_OP_RELABEL;
    m_num_relabels++;

    m_count[m_distance[node]]--;
    m_distance[node] = 2 * m_G->number_of_nodes();

    for (auto e = m_G->get_first_edge(node); e < m_G->get_first_invalid_edge(node); e++) {
      if (m_G->getEdgeCapacity(node, e) > m_G->getEdgeFlow(node, e)) {
        NodeType target = m_G->getEdgeTarget(node, e);
        m_distance[node] = std::min(m_distance[node], m_distance[target] + 1);
      }
      m_work++;
    }

    m_count[m_distance[node]]++;
    enqueue(node);
  }

  FlowType solve_max_flow_min_cut(flow_graph& G, NodeType source, NodeType sink,
                                  bool compute_source_set, std::vector<NodeType>& source_set) {
    TIMED_FUNC(timer);
    m_G = &G;
    m_work = 0;
    m_num_relabels = 0;
    m_gaps = 0;
    m_pushes = 0;
    m_global_updates = 1;

    init(G, source, sink);
    global_relabeling(source, sink);

    long work_todo = WORK_NODE_TO_EDGES * G.number_of_nodes() + G.number_of_edges();
    // main loop
    while (!m_Q.empty()) {
      NodeType v = m_Q.front();
      m_Q.pop();
      m_active[v] = false;
      discharge(v);

      if (m_work > GLOBAL_UPDATE_FRQ * work_todo) {
        global_relabeling(source, sink);
        m_work = 0;
        m_global_updates++;
      }
    }

    if (compute_source_set) {
      // perform bfs starting from source set
      source_set.clear();

      for (NodeType node = 0; node < m_G->number_of_nodes(); node++) {
        m_bfstouched[node] = false;
      }

      std::queue<NodeType> Q;
      Q.push(source);
      m_bfstouched[source] = true;

      while (!Q.empty()) {
        NodeType node = Q.front();
        Q.pop();
        source_set.push_back(node);

        for (auto e = m_G->get_first_edge(node); e < m_G->get_first_invalid_edge(node); e++) {
          NodeType target = G.getEdgeTarget(node, e);
          if (G.getEdgeCapacity(node, e) > G.getEdgeFlow(node, e) && !m_bfstouched[target]) {
            Q.push(target);
            m_bfstouched[target] = true;
          }
        }
      }
    }

    // return value of flow
    return m_excess[sink];
  }

 private:
  std::vector<long long> m_excess;
  std::vector<NodeType> m_distance;
  std::vector<bool> m_active;  // store which nodes are in the queue already
  std::vector<long> m_count;
  std::queue<NodeType> m_Q;
  std::vector<bool> m_bfstouched;
  // highest_label_queue    m_Q;
  long m_num_relabels;
  long m_gaps;
  long m_global_updates;
  long m_pushes;
  long m_work;
  flow_graph* m_G;
};
}  // namespace rpo::edge_orientation::flows
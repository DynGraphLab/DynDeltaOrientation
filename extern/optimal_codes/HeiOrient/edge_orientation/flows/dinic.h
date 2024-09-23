#pragma once

#include <numeric>
#include <queue>
#include <vector>

#include "utils/logging.h"

namespace rpo::edge_orientation::flows {
template <class flow_graph>
struct Dinic {
  using NodeType = typename flow_graph::NodeType;
  using FlowType = typename flow_graph::FlowType;

  const FlowType flow_inf = std::numeric_limits<FlowType>::max();
  flow_graph* m_G;
  NodeType s, t;
  std::vector<FlowType> level;
  std::vector<typename flow_graph::EdgeType> ptr;
  std::vector<NodeType> q;

  Dinic() {}
  FlowType solve_max_flow_min_cut(flow_graph& G, NodeType source, NodeType sink,
                                  bool compute_source_set, std::vector<NodeType>& source_set) {
    s = source;
    t = sink;
    m_G = &G;
    level.resize(G.number_of_nodes());
    ptr.resize(G.number_of_nodes());
    q.resize(G.number_of_nodes());
    return flow();
  }

  bool bfs() {
    size_t i = 0;
    size_t sizeQ = 0;
    q[sizeQ++] = s;
    while (i < sizeQ) {
      auto v = q[i++];
      for (auto e = m_G->get_first_edge(v); e < m_G->get_first_invalid_edge(v); e++) {
        if (m_G->getEdgeCapacity(v, e) > m_G->getEdgeFlow(v, e) &&
            level[m_G->getEdgeTarget(v, e)] == -1) {
          level[m_G->getEdgeTarget(v, e)] = level[v] + 1;
          q[sizeQ++] = (m_G->getEdgeTarget(v, e));
        }
      }
    }
    return level[t] != -1;
  }
  auto tOf_S_T_Cut(flow_graph& G, std::vector<bool>& removed, NodeType source, NodeType target) {
    std::vector<NodeType> setT;
    q.resize(G.number_of_nodes());
    std::vector<bool> visited(G.number_of_nodes());
    visited[target] = true;
    size_t sizeQ = 0;
    q[sizeQ++] = target;
    size_t i = 0;
    while (i < sizeQ) {
      auto v = q[i++];
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
          q[sizeQ++] = neighbor;
          visited[neighbor] = true;
        }
      }
    }
    return setT;
  }

  auto dfs(NodeType v, FlowType pushed) {
    if (pushed == 0) return FlowType{0};
    if (v == t) return pushed;
    for (auto& cid = ptr[v]; cid < m_G->get_first_invalid_edge(v); cid++) {
      auto id = cid;
      auto u = m_G->getEdgeTarget(v, id);
      if (level[v] + 1 == level[u] && m_G->getEdgeCapacity(v, id) > m_G->getEdgeFlow(v, id)) {
        auto tr = dfs(u, std::min(pushed, m_G->getEdgeCapacity(v, id) - m_G->getEdgeFlow(v, id)));
        if (tr > 0) {
          m_G->setEdgeFlow(v, id, m_G->getEdgeFlow(v, id) + tr);
          m_G->setEdgeFlow(
              m_G->getEdgeTarget(v, id), m_G->getReverseEdge(v, id),
              m_G->getEdgeFlow(m_G->getEdgeTarget(v, id), m_G->getReverseEdge(v, id)) - tr);
          return tr;
        }
      }
    }
    return FlowType{0};
  }

  auto flow() {
    TIMED_FUNC(timer);
    FlowType f = 0;
    fill(level.begin(), level.end(), -1);
    level[s] = 0;
    while (bfs()) {
      fill(ptr.begin(), ptr.end(), 0);
      while (long long pushed = dfs(s, flow_inf)) {
        f += pushed;
      }
      fill(level.begin(), level.end(), -1);
      level[s] = 0;
    }
    return f;
  }
};
}  // namespace rpo::edge_orientation::flows
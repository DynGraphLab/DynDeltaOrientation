#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <utility>
#include <vector>

#include "absl/strings/string_view.h"
#include "edge_orientation/ds/flow_graph.h"
#include "edge_orientation/flows/dinic.h"
#include "utils/range.h"

namespace rpo {
namespace edge_orientation {
namespace blumenstock {
namespace {
using rpo::utils::Range;
using rpo::utils::SimpleIterator;

}  // namespace
template <class flow_graph = rpo::edge_orientation::ds::FlowGraph<unsigned int, size_t, long>>
class BlumenstockPushRelabelDS {
 public:
  using NodeType = typename flow_graph::NodeType;
  using EdgeType = typename flow_graph::EdgeType;

 private:
  flow_graph graph;

  static constexpr NodeType t = 1;
  static constexpr NodeType s = 0;

  size_t edgeId = 0;
  size_t maxDegree;
  typename flow_graph::EdgeType edge_count;
  NodeType vertex_count;
  std::vector<bool> orientation_map;  // pair 0<1
  std::vector<typename flow_graph::FlowType> degs;

 public:
  static constexpr absl::string_view ds_name = "blumenstock";
  BlumenstockPushRelabelDS(typename flow_graph::EdgeType edge_c, NodeType vertex_c)
      : orientation_map(edge_c, false), degs(vertex_c + 3, 0), instance_(vertex_c, edge_c) {
    edge_count = edge_c;

    vertex_count = vertex_c;
    graph.start_construction(vertex_count + 3, 0);
    for (NodeType v = 0; v < vertex_count; v++) {
      graph.new_edge(s, v + 3, 0);  // some default value
    }
    for (NodeType v = 0; v < vertex_count; v++) {
      graph.new_edge(v + 3, t, 0);  // some default value, now the first two edges are invalid
    }
  }
  size_t edge_id = 0;
  /// assumes sorted edges
  void addEdge(NodeType u, NodeType v, NodeType w) {
    // std::cout << u << " " << v << " " << edgeId << std::endl;
    //  assert(edgeId < vertex_count + edge_count + 2);
    if (u < v) {  // expecting to get undirected graph
                  // map[edgeId++] = std::make_pair(u + 3, v + 3);
      edge_count++;
      graph.new_edge(u + 3, v + 3, 1);  // add one orientation

      degs[u + 3]++;
      // graph.new_edge(v + 3, u + 3, 0);  // and one empty implicity
    } else {
      edge_id++;
    }
  }
  template <typename Iterator>
  void addEdges(NodeType u, Iterator b, Iterator e) {
    for (auto v : Range<Iterator>(b, e)) {
      if (v > u) {
        break;
      }
      addEdge(u, v, 1);
      addEdge(v, u, 1);
    }
  }
  void shrink_to_size() { edge_count = edge_id; }
  template <class solver_class>
  int reorient(typename flow_graph::FlowType Target, bool& ran, solver_class& push_solver) {
    TIMED_FUNC(timer);
    size_t connections_from_s = 0;
    size_t edge_id = 0;
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      auto outlier_edge = 1;
      auto incoming_edge = graph.get_first_edge(s) + v - 3;
      // reset edges
      graph.setEdgeFlow(s, incoming_edge, 0);
      graph.setEdgeCapacity(s, incoming_edge, 0);

      graph.setEdgeFlow(v, outlier_edge, 0);
      graph.setEdgeCapacity(v, outlier_edge, 0);

      auto d_v = degs[v];

      if (d_v > Target) {
        connections_from_s++;
        graph.setEdgeCapacity(s, incoming_edge, d_v - Target);
      }
      if (d_v < Target) {
        graph.setEdgeCapacity(v, outlier_edge, Target - d_v);
      }
      graph.setEdgeFlow(v, 0, 0);
      graph.setEdgeFlow(v, 1, 0);
      for (auto e = graph.get_first_edge(v) + 2; e < graph.get_first_invalid_edge(v); e++) {
        graph.setEdgeFlow(v, e, 0);
        const auto u = graph.getEdgeTarget(v, e);
        if (v > u) {
          if (orientation_map[edge_id]) {
            // update capacites
            graph.setEdgeCapacity(v, e, 1);                           // original edge is no flow
            graph.setEdgeCapacity(u, graph.getReverseEdge(v, e), 0);  // reverse yes
          } else {
            // update capacites
            graph.setEdgeCapacity(v, e, 0);                           // original edge is no flow
            graph.setEdgeCapacity(u, graph.getReverseEdge(v, e), 1);  // reverse yes
          }
          edge_id++;
        }
      }
    }
    if (connections_from_s == 0) {
      ran = true;
      return Target;  // did not change a thing
    }

    std::vector<NodeType> _emptyref;
    auto res = push_solver.solve_max_flow_min_cut(graph, s, t, false, _emptyref);
    typename flow_graph::FlowType capSourceArcs = 0;
    for (auto edge = graph.get_first_edge(s); edge < graph.get_first_invalid_edge(s); edge++) {
      auto c = graph.getEdgeCapacity(s, edge);
      capSourceArcs += c;
    }
    ran = res == capSourceArcs;
    edge_id = 0;
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      for (auto e = graph.get_first_edge(s) + 2; e < graph.get_first_invalid_edge(v); e++) {
        const auto u = graph.getEdgeTarget(v, e);
        if (v > u) {
          if (graph.getEdgeFlow(v, e) == 1 ||
              graph.getEdgeFlow(u, graph.getReverseEdge(v, e)) == 1) {
            // re orient
            // std::cout << "Changing " << v << " " << u << std::endl;
            orientation_map[edge_id] = !orientation_map[edge_id];
            if (orientation_map[edge_id]) {
              degs[v] += 1;
              degs[u] -= 1;
            } else {
              degs[v] -= 1;
              degs[u] += 1;
            }
            graph.setEdgeFlow(v, e, 0);
            graph.setEdgeFlow(u, graph.getReverseEdge(v, e), 0);
          }

          edge_id++;
        }
      }
    }
    typename flow_graph::FlowType max_deg = 0;

    for (auto c : degs) {
      max_deg = std::max(max_deg, c);
    }
    // std::cout << " res " << max_deg << " o" << maxOutdegree() << std::endl;
    return max_deg;  // have to rerun
  }
  template <class solver_class = rpo::edge_orientation::flows::Dinic<flow_graph>>
  void solve() {
    int left = (size_t)std::ceil(((double)edge_count) / ((double)vertex_count));
    int right = vertex_count;  // trivial count
    int sqrtBound = std::floor((std::sqrt(8 * edge_count - 7.0) + 3.0) / 4.0);
    if (sqrtBound < right) {
      right = sqrtBound;
    }
    typename flow_graph::FlowType max_deg = 0;
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      max_deg = std::max(max_deg, (typename flow_graph::FlowType)graph.get_first_invalid_edge(v));
    }
    max_deg = std::ceil(((double)max_deg - 2) / 2.0);
    if (max_deg < right) {
      right = max_deg;
    }
    const auto m_d = maxOutdegree();
    if (m_d < right) {
      right = m_d;
    }
    do {
      auto test_point = std::ceil(((double)(right + left)) * 0.5);
      bool fin = false;
      solver_class solver;

      auto new_orient = reorient<solver_class>(test_point, fin, solver);
      //  std::cout << "new: " << new_orient << " t:" << test_point << " " << fin << std::endl;
      if (fin) {
        right = test_point - 1;

      } else {
        left = test_point + 1;
      }
      // std::cout << "Left " << left << " right: " << right << " d=" << maxOutdegree() <<
      // std::endl;

    } while (right - left >= 0);
  }
  void initialFlowNetworkForGeorgakopoulosPolitopoulosUnweightedIntegral(
      flow_graph& graph, typename flow_graph::FlowType lambda) {
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      auto outlier_edge = 1;
      auto incoming_edge = graph.get_first_edge(s) + v - 3;
      // reset edges
      graph.setEdgeFlow(s, incoming_edge, 0);
      graph.setEdgeCapacity(s, incoming_edge, 0);

      graph.setEdgeFlow(v, outlier_edge, 0);
      graph.setEdgeCapacity(v, outlier_edge, 0);
      degs[v] = graph.get_first_invalid_edge(v) - 2;
      typename flow_graph::FlowType d_v = 0;
      for (auto e = graph.get_first_edge(v) + 2; e < graph.get_first_invalid_edge(v); e++) {
        graph.setEdgeFlow(v, e, 0);
        graph.setEdgeCapacity(v, e, 2 * vertex_count);
      }
      for (auto e = graph.get_first_edge(v) + 2; e < graph.get_first_invalid_edge(v); e++) {
        d_v += graph.getEdgeCapacity(v, e);
      }

      if (d_v > lambda) {
        graph.setEdgeCapacity(s, incoming_edge, d_v - lambda);
      }
      if (d_v < lambda) {
        graph.setEdgeCapacity(v, outlier_edge, lambda - d_v);
      }
      graph.setEdgeFlow(v, 0, 0);
      graph.setEdgeFlow(v, 1, 0);
    }
  }
  void initialFlowNetworkForGoldbergUnweightedIntegral(flow_graph& graph,
                                                       typename flow_graph::FlowType guesstimes2,
                                                       typename flow_graph::FlowType numEdges) {
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      auto outlier_edge = 1;
      auto incoming_edge = graph.get_first_edge(s) + v - 3;
      // reset edges
      graph.setEdgeFlow(s, incoming_edge, 0);
      graph.setEdgeCapacity(s, incoming_edge, 0);

      graph.setEdgeFlow(v, outlier_edge, 0);
      graph.setEdgeCapacity(v, outlier_edge, 0);
      degs[v] = graph.get_first_invalid_edge(v) - 2;
      typename flow_graph::FlowType d_v = degs[v];
      for (auto e = graph.get_first_edge(v) + 2; e < graph.get_first_invalid_edge(v); e++) {
        graph.setEdgeFlow(v, e, 0);
        graph.setEdgeCapacity(v, e, 1);
      }

      graph.setEdgeCapacity(s, incoming_edge, numEdges);

      graph.setEdgeCapacity(v, outlier_edge, numEdges + guesstimes2 - d_v);

      graph.setEdgeFlow(v, 0, 0);
      graph.setEdgeFlow(v, 1, 0);
    }
  }
  void updateGoldberg(flow_graph& graph, typename flow_graph::FlowType guesstimes2,
                      typename flow_graph::FlowType numEdges) {
    for (typename flow_graph::NodeType u = 3; u < vertex_count + 3; u++) {
      auto incoming_edge = graph.get_first_edge(s) + u - 3;
      auto outlier_edge = 1;
      graph.setEdgeFlow(s, incoming_edge, 0);
      graph.setEdgeFlow(u, 0, 0);
      graph.setEdgeFlow(u, outlier_edge, 0);

      typename flow_graph::FlowType d_v = 0;
      for (auto e = graph.get_first_edge(u) + 2; e < graph.get_first_invalid_edge(u); e++) {
        d_v += graph.getEdgeCapacity(u, e);
        graph.setEdgeFlow(u, e, 0);
      }

      graph.setEdgeCapacity(s, incoming_edge, numEdges);

      graph.setEdgeCapacity(u, outlier_edge, numEdges + guesstimes2 - d_v);
    }
  }
  void updateMargins(flow_graph& graph, std::vector<bool>& removed,
                     typename flow_graph::FlowType lambda) {
    for (typename flow_graph::NodeType u = 3; u < vertex_count + 3; u++) {
      auto incoming_edge = graph.get_first_edge(s) + u - 3;
      graph.setEdgeFlow(s, incoming_edge, 0);
      graph.setEdgeFlow(u, 0, 0);

      auto outlier_edge = 1;
      typename flow_graph::FlowType d_v = 0;
      for (auto e = graph.get_first_edge(u) + 2; e < graph.get_first_invalid_edge(u); e++) {
        d_v += graph.getEdgeCapacity(u, e);
      }
      graph.setEdgeCapacity(s, incoming_edge, 0);
      graph.setEdgeCapacity(u, outlier_edge, 0);
      if (removed[u]) {
        continue;
      }
      if (d_v > lambda) {
        graph.setEdgeCapacity(s, incoming_edge, d_v - lambda);
      }
      if (d_v < lambda) {
        graph.setEdgeCapacity(u, outlier_edge, lambda - d_v);
      }
    }
  }
  template <class solver_class = rpo::edge_orientation::flows::Dinic<flow_graph>>
  int solve_goldberg(bool produceOrientation) {
    TIMED_FUNC(timer);
    // std::cout << edge_count << " " << vertex_count << std::endl;
    typename flow_graph::FlowType lambdaMin =
        2 * std::ceil(((double)edge_count) /
                      ((double)vertex_count));  // lower value for two times guess
    typename flow_graph::FlowType lambdaMax =
        (typename flow_graph::FlowType)(vertex_count / 2);  // trivial count
    typename flow_graph::FlowType sqrtBound =
        (typename flow_graph::FlowType)std::floor((std::sqrt(8.0 * edge_count - 7.0) + 3.0) / 4.0);
    if (sqrtBound < lambdaMax) {
      lambdaMax = sqrtBound;
    }
    typename flow_graph::FlowType max_deg = 0;
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      max_deg = std::max(max_deg, (typename flow_graph::FlowType)(graph.get_first_invalid_edge(v) -
                                                                  graph.get_first_edge(v)));
    }
    // std::cout << max_deg << std::endl;
    //  assert(false);
    max_deg = (typename flow_graph::FlowType)std::ceil(((double)max_deg - 2.0) / 2.0);
    // std::cout << max_deg << std::endl;

    if (max_deg < lambdaMax) {
      lambdaMax = max_deg;
    }
    auto graph2 = graph;
    auto minFeasible = lambdaMax;
    lambdaMax *= 2;  // multiplied by 2 since Goldberg method works with factor 2

    typename flow_graph::FlowType lambda = std::ceil((lambdaMax + lambdaMin) / 2.0);

    // initialize
    // std::cout << lambdaMin << " l: " << lambda << " " << lambdaMax << std::endl;
    std::vector<bool> removed(vertex_count + 3);
    auto currentVSize = vertex_count;
    auto currentNumEdges = edge_count;
    initialFlowNetworkForGoldbergUnweightedIntegral(graph2, lambda, currentNumEdges);
    while ((lambdaMax > (lambdaMin + 1))) {
      solver_class push_solver;

      std::vector<NodeType> _emptyref;
      auto cutSize = push_solver.solve_max_flow_min_cut(graph2, s, t, false, _emptyref);
      auto cut = push_solver.tOf_S_T_Cut(graph2, removed, s, t);

      // std::cout << cut.size() << "  rest: " << currentVSize << std::endl;
      if (cut.size() == currentVSize) {
        // std::cout << "SUCCESS " << lambda / (2.0 * vertex_count) << std::endl;
        lambdaMax = lambda;
      } else {
        lambdaMin = lambda;
        // Delete by bfs
        // std::cout << cut.size() << " " << cutSize << " " << currentVSize << std::endl;

        /*for (auto c : cut) {
          //  std::cout << "rem: " << c << std::endl;
          removed[c] = true;
          graph2.setEdgeCapacity(s, c - 3, 0);
          graph2.setEdgeFlow(s, c - 3, 0);

          // degs[c] = 0;
          for (auto edge = graph2.get_first_edge(c) + 2; edge < graph2.get_first_invalid_edge(c);
               edge++) {
            auto u = graph2.getEdgeTarget(c, edge);
            auto r = graph2.getReverseEdge(c, edge);
            currentNumEdges--;
            graph2
                .m_adjacency_lists[graph2.m_adjacency_lists[u].back().target]
                                  [graph2.m_adjacency_lists[u].back().reverse_edge_index]
                .reverse_edge_index = r;
            graph2.m_adjacency_lists[u][r] = graph2.m_adjacency_lists[u].back();
            graph2.m_adjacency_lists[u].pop_back();
          }
          graph2.m_adjacency_lists[c].resize(2);
        }
        currentVSize -= cut.size();*/
      }
      lambda = std::floor(((double)lambdaMax + lambdaMin) / 2.0);
      std::cout << "lambda: " << lambda << " " << std::ceil(((double)lambdaMax + lambdaMin) / 2.0)
                << std::endl;
      //   std::cout << "R:" << ((double)currentNumEdges / (double)currentVSize) << std::endl;

      updateGoldberg(graph2, lambda, currentNumEdges);
    }
    // std::cout << lambda << std::endl;
    // std::cout << lambda / (2.0f * vertex_count) << std::endl;
    // std::cout << minFeasible << std::endl;
    //   std::cout << "R:" << ((double)currentNumEdges / (double)currentVSize) << std::endl;
    std::cout << "lambda: " << lambda << " " << std::ceil(((double)lambdaMax + lambdaMin) / 2.0)
              << std::endl;
    minFeasible = std::ceil(((double)lambda) / 2.0);
    // std::cout << "MIN:" << minFeasible << " " << lambdaMax << " " << lambdaMin << std::endl;
    if (produceOrientation) {
      bool test = false;
      solver_class push_solver;
      std::fill(degs.begin(), degs.end(), 0);
      for (NodeType v = 3; v < vertex_count + 3; v++) {
        for (auto e = graph.get_first_edge(s) + 2; e < graph.get_first_invalid_edge(v); e++) {
          const auto u = graph.getEdgeTarget(v, e);
          if (v > u) {
            // re orient
            // std::cout << "Changing " << v << " " << u << std::endl;
            degs[u] += 1;
          }
        }
      }
      reorient(minFeasible, test, push_solver);
    } else {
      _ran_gp = true;
      result_by_gp = minFeasible;
    }
    return minFeasible;
  }
  void updateFlowListsAsInFlowNetworkAndClearFlow(flow_graph& graph, std::vector<bool>& removed,
                                                  bool was_unsuccessfull) {
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      if (removed[v]) {
        continue;
      }
      auto outlier_edge = 1;
      auto incoming_edge = graph.get_first_edge(s) + v - 3;

      // reset edges
      graph.setEdgeFlow(s, incoming_edge, 0);
      graph.setEdgeFlow(v, 0, 0);
      graph.setEdgeFlow(v, 1, 0);

      // graph.setEdgeCapacity(s, incoming_edge, 0);
      graph.setEdgeFlow(s, incoming_edge, 0);

      graph.setEdgeFlow(v, outlier_edge, 0);
      graph.setEdgeCapacity(v, outlier_edge, 0);
      if (was_unsuccessfull) {
        for (auto e = graph.get_first_edge(v) + 2; e < graph.get_first_invalid_edge(v); e++) {
          typename flow_graph::FlowType edge_cap = graph.getEdgeCapacity(v, e);
          const typename flow_graph::FlowType flow = graph.getEdgeFlow(v, e);
          if (flow > 0) {
            graph.setEdgeCapacity(v, e, edge_cap - flow);
            graph.setEdgeCapacity(
                graph.getEdgeTarget(v, e), graph.getReverseEdge(v, e),
                graph.getEdgeCapacity(graph.getEdgeTarget(v, e), graph.getReverseEdge(v, e)) +
                    flow);
          }
        }
      }
    }
    for (NodeType v = 0; v < vertex_count + 3; v++) {
      for (auto e = graph.get_first_edge(v); e < graph.get_first_invalid_edge(v); e++) {
        graph.setEdgeFlow(v, e, 0);
      }
    }
  }
  template <class solver_class = rpo::edge_orientation::flows::Dinic<flow_graph>>
  int solve_pseudo(bool produceOrientation) {
    TIMED_FUNC(timer);
    // std::cout << edge_count << " " << vertex_count << std::endl;
    typename flow_graph::FlowType lambdaMin =
        std::ceil(((double)edge_count) / ((double)vertex_count));
    typename flow_graph::FlowType lambdaMax =
        (typename flow_graph::FlowType)(vertex_count / 2);  // trivial count
    typename flow_graph::FlowType sqrtBound =
        (typename flow_graph::FlowType)std::floor((std::sqrt(8.0 * edge_count - 7.0) + 3.0) / 4.0);
    if (sqrtBound < lambdaMax) {
      lambdaMax = sqrtBound;
    }
    typename flow_graph::FlowType max_deg = 0;
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      max_deg = std::max(max_deg, (typename flow_graph::FlowType)(graph.get_first_invalid_edge(v) -
                                                                  graph.get_first_edge(v)));
    }
    // std::cout << max_deg << std::endl;
    //  assert(false);
    max_deg = (typename flow_graph::FlowType)std::ceil(((double)max_deg - 2.0) / 2.0);
    // std::cout << max_deg << std::endl;

    if (max_deg < lambdaMax) {
      lambdaMax = max_deg;
    }
    auto graph2 = graph;
    auto minFeasible = lambdaMax;
    lambdaMax--;
    lambdaMin = 2 * edge_count;                 // 2*W(E)
    lambdaMax = 4 * edge_count * vertex_count;  // maybe use better bounds // 2W(E)W(V)
    typename flow_graph::FlowType lambda = std::ceil((lambdaMax + lambdaMin) / 2.0);

    // initialize
    // std::cout << lambdaMin << " l: " << lambda << " " << lambdaMax << std::endl;
    std::vector<bool> removed(vertex_count + 3);
    auto currentVSize = vertex_count;
    auto currentNumEdges = edge_count;
    initialFlowNetworkForGeorgakopoulosPolitopoulosUnweightedIntegral(graph2, lambda * 2);
    while ((lambdaMax - lambdaMin) > 1) {
      solver_class push_solver;

      std::vector<NodeType> _emptyref;
      auto cutSize = push_solver.solve_max_flow_min_cut(graph2, s, t, false, _emptyref);
      auto cut = push_solver.tOf_S_T_Cut(graph2, removed, s, t);

      // std::cout << cut.size() << "  rest: " << currentVSize << std::endl;
      if (cut.size() == currentVSize) {
        // std::cout << "SUCCESS " << lambda / (2.0 * vertex_count) << std::endl;
        lambdaMax = lambda;
      } else {
        lambdaMin = lambda;
        // Delete by bfs
        // std::cout << cut.size() << " " << cutSize << " " << currentVSize << std::endl;

        for (auto c : cut) {
          //  std::cout << "rem: " << c << std::endl;
          removed[c] = true;
          graph2.setEdgeCapacity(s, c - 3, 0);
          graph2.setEdgeFlow(s, c - 3, 0);

          // degs[c] = 0;
          for (auto edge = graph2.get_first_edge(c) + 2; edge < graph2.get_first_invalid_edge(c);
               edge++) {
            auto u = graph2.getEdgeTarget(c, edge);
            auto r = graph2.getReverseEdge(c, edge);
            currentNumEdges--;
            graph2
                .m_adjacency_lists[graph2.m_adjacency_lists[u].back().target]
                                  [graph2.m_adjacency_lists[u].back().reverse_edge_index]
                .reverse_edge_index = r;
            graph2.m_adjacency_lists[u][r] = graph2.m_adjacency_lists[u].back();
            graph2.m_adjacency_lists[u].pop_back();
          }
          graph2.m_adjacency_lists[c].resize(2);
        }
        currentVSize -= cut.size();
      }
      lambda = std::floor(((double)lambdaMax + lambdaMin) / 2.0);
      // std::cout << "lambda: " << lambda << " Size: " << lambda / (2.0 * vertex_count) <<
      // std::endl;
      //   std::cout << "R:" << ((double)currentNumEdges / (double)currentVSize) << std::endl;

      updateFlowListsAsInFlowNetworkAndClearFlow(graph2, removed, cut.size() != currentVSize);

      updateMargins(graph2, removed, lambda * 2);
    }
    // std::cout << lambda << std::endl;
    // std::cout << lambda / (2.0f * vertex_count) << std::endl;
    // std::cout << minFeasible << std::endl;
    //   std::cout << "R:" << ((double)currentNumEdges / (double)currentVSize) << std::endl;
    minFeasible = std::ceil(((double)currentNumEdges) / ((double)currentVSize));
    // std::cout << "MIN:" << minFeasible << " " << lambdaMax << " " << lambdaMin << std::endl;
    if (produceOrientation) {
      bool test = false;
      solver_class push_solver;
      std::fill(degs.begin(), degs.end(), 0);
      for (NodeType v = 3; v < vertex_count + 3; v++) {
        for (auto e = graph.get_first_edge(s) + 2; e < graph.get_first_invalid_edge(v); e++) {
          const auto u = graph.getEdgeTarget(v, e);
          if (v > u) {
            // re orient
            // std::cout << "Changing " << v << " " << u << std::endl;
            degs[u] += 1;
          }
        }
      }
      reorient(minFeasible, test, push_solver);
    } else {
      _ran_gp = true;
      result_by_gp = minFeasible;
    }
    return minFeasible;
  }
  template <class solver_class = rpo::edge_orientation::flows::Dinic<flow_graph>>
  void solve_repeat() {
    TIMED_FUNC(timer);
    typename flow_graph::FlowType left = std::ceil(((double)edge_count) / ((double)vertex_count));
    typename flow_graph::FlowType right =
        (typename flow_graph::FlowType)(vertex_count / 2);  // trivial count

    typename flow_graph::FlowType sqrtBound =
        (typename flow_graph::FlowType)std::floor((std::sqrt(8 * edge_count - 7.0) + 3.0) / 4.0);
    if (sqrtBound < right) {
      right = sqrtBound;
    }
    typename flow_graph::FlowType max_deg = 0;
    for (NodeType v = 3; v < vertex_count + 3; v++) {
      max_deg = std::max(max_deg, (typename flow_graph::FlowType)(graph.get_first_invalid_edge(v) -
                                                                  graph.get_first_edge(v)));
    }
    // std::cout << max_deg << std::endl;
    //  assert(false);
    max_deg = (typename flow_graph::FlowType)std::ceil(((double)max_deg - 2.0) / 2.0);
    // std::cout << max_deg << std::endl;

    if (max_deg < right) {
      right = max_deg;
    }
    typename flow_graph::FlowType m_d = maxOutdegree();
    if (m_d < right) {
      right = m_d;
    }
    solver_class solver;

    do {
      typename flow_graph::FlowType test_point =
          static_cast<typename flow_graph::FlowType>(std::ceil(((double)(right + left)) * 0.5));
      bool fin = false;

      auto new_orient = reorient<solver_class>(test_point, fin, solver);
      // std::cout << "new: " << new_orient << " t:" << test_point << " " << fin << std::endl;
      if (fin) {
        right = test_point - 1;

      } else {
        left = test_point + 1;
      }
      // std::cout << "Left " << left << " right: " << right << " d=" << maxOutdegree() <<
      // std::endl;

    } while (right - left >= 0);
  }
  bool _ran_gp = false;
  typename flow_graph::FlowType result_by_gp = 0;
  typename flow_graph::FlowType maxOutdegree() {
    if (_ran_gp) {
      return result_by_gp;
    }
    typename flow_graph::FlowType maxOut = 0;
    for (auto c : degs) {
      maxOut = std::max(maxOut, c);
    }
    return maxOut;
  }

  int size() { return maxOutdegree(); }
  int weight() { return size(); }
  double quality() { return 0; }
  bool valid() { return true; }
  int free_edges_size() {
    return 0;  // not applicable
  }
  struct InstanceImpl {
    NodeType _n, _e;
    InstanceImpl(NodeType n, NodeType e) : _n(n), _e(e) {}
    auto currentNumNodes() const { return _n; }
    auto currentNumEdges() const { return _e; }
  };
  InstanceImpl instance_;
  InstanceImpl& instance() { return instance_; }
  void setNodeWeight(NodeType, NodeType) {}
  void setEdgeWeight(NodeType, NodeType) {}

  using WeightType = NodeType;
  auto edges() const {
    return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(edge_count));
  }
  auto nodes() const {
    return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(vertex_count));
  }
  auto edgeSize(NodeType) { return 1; }
  void finish() { shrink_to_size(); }
};
}  // namespace blumenstock
}  // namespace edge_orientation
}  // namespace rpo
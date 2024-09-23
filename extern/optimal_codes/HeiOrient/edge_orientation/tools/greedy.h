#pragma once

#include <tuple>

#include "utils/logging.h"

namespace rpo {
namespace edge_orientation {
namespace greedy {
template <class FlipGraph>
void solve_up_to_length_one(FlipGraph& instance) {
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;
  TIMED_FUNC(tere);

  bool improvement_found_overalls2 = false;
  // TODO update max_flows
  // while (impr)

  auto max_flow = instance.computeOutflows();
  // std::cout << max_flow << " at " << max_flow_index << std::endl;
  do {
    improvement_found_overalls2 = false;
    IndexType currNode{0};
    for (auto& [outflow_v, incEdges] : instance._nodes) {
      auto v = currNode++;
      bool improvement_found_overalls = false;
      if (outflow_v < max_flow) {
        continue;
      }
      auto old_f = max_flow;
      // precheck
      bool impr = false;
      int improvements = 0;
      // do {
      impr = false;
      const auto& [start_edges, count_u] = incEdges[0];

      for (size_t i = start_edges; i < count_u; i++) {
        if (std::get<0>(instance._nodes[std::get<1>(instance._edges[instance.flat_edge_info[i]])]) <
            outflow_v - 1) {
          improvements++;
          instance.flip_edge(instance.flat_edge_info[i]);
          impr = true;
          improvement_found_overalls = true;
          break;
        }
      }
      if (impr) {
        continue;
      }
      auto& [start_v, count_v] = incEdges[1];

      for (size_t i = start_v; i < count_v; i++) {
        if (std::get<0>(instance._nodes[std::get<0>(instance._edges[instance.flat_edge_info[i]])]) <
            outflow_v - 1) {
          improvements++;
          instance.flip_edge(instance.flat_edge_info[i]);
          impr = true;
          improvement_found_overalls = true;
          break;
        }
      }
      if (impr) {
        continue;
      }
      // } while (impr);

      for (auto e1x = instance.flat_edge_info.begin() + incEdges[0].first;
           e1x < instance.flat_edge_info.begin() + incEdges[0].second; e1x++) {
        // flow is in this direction
        // look at other side
        auto u = std::get<1>(instance._edges[*e1x]);
        auto& [outflow_u, incEdgesMiddle] = instance._nodes[u];
        if (outflow_u >= outflow_v) {
          continue;
        }
        {
          bool two_path = false;
          {
            auto count_v = incEdgesMiddle[0].second;
            for (size_t i = incEdgesMiddle[0].first; i < count_v; i++) {
              if (std::get<0>(
                      instance._nodes[std::get<1>(instance._edges[instance.flat_edge_info[i]])]) <
                  outflow_v - 1) {
                improvements++;
                instance.flip_edge(instance.flat_edge_info[i]);
                instance.flip_edge(*e1x);

                impr = true;
                two_path = true;

                improvement_found_overalls = true;
                break;
              }
            }
          }

          if (!two_path) {
            auto& count_v = incEdgesMiddle[1].second;
            for (size_t i = incEdgesMiddle[1].first; i < count_v; i++) {
              if (std::get<0>(
                      instance._nodes[std::get<0>(instance._edges[instance.flat_edge_info[i]])]) <
                  outflow_v - 1) {
                instance.flip_edge(instance.flat_edge_info[i]);
                instance.flip_edge(*e1x);
                two_path = true;
                impr = true;
                improvement_found_overalls = true;

                //  checkMaxFlow(max_flow);
                break;
              }
            }
          }
          if (impr) {
            break;
          }
        }
      }
      if (impr) {
        continue;
      }
      for (auto e1x = instance.flat_edge_info.begin() + incEdges[1].first;
           e1x < instance.flat_edge_info.begin() + incEdges[1].second; e1x++) {
        // flow is in this direction
        // look at other side
        auto u = std::get<0>(instance._edges[*e1x]);
        auto& [outflow_u, incEdgesMiddle] = instance._nodes[u];
        if (outflow_u >= outflow_v) {
          continue;
        }
        {
          bool two_path = false;
          {
            auto count_v = incEdgesMiddle[0].second;
            for (size_t i = incEdgesMiddle[0].first; i < count_v; i++) {
              if (std::get<0>(
                      instance._nodes[std::get<1>(instance._edges[instance.flat_edge_info[i]])]) <
                  outflow_v - 1) {
                instance.flip_edge(*e1x);
                instance.flip_edge(instance.flat_edge_info[i]);
                impr = true;
                two_path = true;
                improvement_found_overalls = true;
                // checkMaxFlow(max_flow);
                break;
              }
            }
          }
          if (!two_path) {
            auto count_v = incEdgesMiddle[1].second;
            for (size_t i = incEdgesMiddle[1].first; i < count_v; i++) {
              if (std::get<0>(
                      instance._nodes[std::get<0>(instance._edges[instance.flat_edge_info[i]])]) <
                  outflow_v - 1) {
                instance.flip_edge(*e1x);
                instance.flip_edge(instance.flat_edge_info[i]);
                two_path = true;
                impr = true;
                improvement_found_overalls = true;

                //  checkMaxFlow(max_flow);
                break;
              }
            }
          }
        }
        if (impr) {
          break;
        }
      }
    }
    if (improvement_found_overalls2) {
      auto new_flow = instance.computeOutflows();
      if (new_flow == max_flow) {
        break;
      }
      max_flow = new_flow;
    }
  } while (improvement_found_overalls2);
}
}  // namespace greedy
}  // namespace edge_orientation
}  // namespace rpo
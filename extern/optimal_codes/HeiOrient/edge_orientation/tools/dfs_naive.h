#pragma once
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include "edge_orientation/ds/smaller_graph.h"
#include "utils/range.h"

namespace rpo {
namespace edge_orientation {
namespace dfs_naive {

template <class FlipGraph>
std::enable_if_t<not rpo::edge_orientation::ds::is_smaller_type<FlipGraph>::value, bool>
improve_paths_stack(
    FlipGraph& instance, typename FlipGraph::IndexType v, typename FlipGraph::OutflowType max_flow,
    std::vector<std::pair<typename FlipGraph::IndexType, typename FlipGraph::InsideArrayRefType>>&
        stack,
    std::vector<typename FlipGraph::InsideArrayRefType>& edge_path,
    std::vector<bool>& visited_array) {
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;
  long stack_pointer = 0;
  stack.resize(1);
  edge_path.resize(1);

  stack[0] = std::make_pair(v, InsideArrayRefType{0});
  bool found = false;
  while (stack_pointer >= 0) {
    if (stack_pointer >= ((long)stack.size()) - 1) {
      edge_path.resize(stack_pointer + 2);
      stack.resize(stack_pointer + 2);
    }
    auto [v, index] = stack[stack_pointer];

    if (instance.outflow(v) < max_flow - 1) {
      visited_array[v] = false;
      found = true;
      break;
    }
    visited_array[v] = true;

    auto [start_v, count_v] = instance.incEdges(v, 1);
    auto [start_u, count_u] = instance.incEdges(v, 0);

    bool deeper = false;

    // next search depth
    for (InsideArrayRefType i = start_u + index; i < count_u; i++) {
      auto e = instance.flat_edge_info[i];
      auto w = instance.nodeInEdge1(e);
      if (visited_array[w] == 0) {
        stack[stack_pointer].second = i + InsideArrayRefType{1} - start_u;
        edge_path[stack_pointer] = e;
        stack[++stack_pointer] = std::make_pair(w, InsideArrayRefType{0});  // go to next
        deeper = true;
        break;
      }
    }
    if (deeper) {
      continue;
    }
    if (index < (count_u - start_u)) {
      index = (count_u - start_u);
    }
    for (InsideArrayRefType i = start_v + index - (count_u - start_u); i < count_v; i++) {
      auto e = instance.flat_edge_info[i];
      auto w = instance.nodeInEdge0(e);
      if (visited_array[w] == 0) {
        stack[stack_pointer].second = (count_u - start_u) + (i - start_v) + InsideArrayRefType{1};
        edge_path[stack_pointer] = e;
        stack[++stack_pointer] = std::make_pair(w, InsideArrayRefType{0});  // go to next
        deeper = true;
        break;
      }
    }
    if (deeper) {
      continue;
    }
    stack_pointer--;
  }

  // unwind stack
  if (found) {
    for (auto e : rpo::utils::Range(edge_path.rend() - stack_pointer, edge_path.rend())) {
      instance.flip_edge(e);
    }
    return true;
  }
  // visited[v] = false;
  return false;
}

template <class FlipGraph>
std::enable_if_t<rpo::edge_orientation::ds::is_smaller_type<FlipGraph>::value, bool>
improve_paths_stack(
    FlipGraph& instance, typename FlipGraph::IndexType og_v,
    typename FlipGraph::OutflowType max_flow,
    std::vector<std::pair<typename FlipGraph::IndexType, typename FlipGraph::InsideArrayRefType>>&
        stack,
    std::vector<typename FlipGraph::InsideArrayRefType>& edge_path,
    std::vector<bool>& visited_array) {
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;
  long stack_pointer = 0;
  stack.resize(1);
  edge_path.resize(1);

  stack[0] = std::make_pair(og_v, instance._nodes_offset[og_v]);
  bool found = false;
  while (stack_pointer >= 0) {
    if (stack_pointer >= ((long)stack.size()) - 1) {
      edge_path.resize(stack_pointer + 2);
      stack.resize(stack_pointer + 2);
    }
    auto [v, index] = stack[stack_pointer];
    if (instance.outflow(v) < max_flow - 1) {
      visited_array[v] = false;
      found = true;
      break;
    }
    visited_array[v] = true;

    bool deeper = false;

    // next search depth
    for (auto i = index; i < instance._nodes_offset[v] + InsideArrayRefType{instance._nodes_out[v]};
         i++) {
      auto w = instance.flat_edge_info[i];
      if (visited_array[w] == 0) {
        stack[stack_pointer].second = i;
        stack[++stack_pointer] = std::make_pair(w, instance._nodes_offset[w]);  // go to next
        deeper = true;
        break;
      }
    }
    if (deeper) {
      continue;
    }
    stack_pointer--;
    if (stack_pointer >= 0) {
      stack[stack_pointer].second++;
    }
  }

  // unwind stack
  if (found) {
    for (auto [u, e] : rpo::utils::Range(stack.rend() - stack_pointer, stack.rend())) {
      instance.flip_active_edge(u, e);
    }
    return true;
  }
  // visited[v] = false;
  return false;
}

template <class FlipGraph>
void solve_by_dfs(FlipGraph& instance, bool reset_every_time) {
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;
  TIMED_FUNC(tere);
  typename FlipGraph::OutflowType lower_bound =
      std::ceil(instance.edge_count() / (double)instance.vertex_count());

  bool path_improved = false;
  bool change_happening = false;
  auto max_flow = instance.computeOutflows();
  if (max_flow == lower_bound) {
    return;
  }
  std::vector<bool> visited_array(instance.vertex_count(), false);
  std::vector<std::pair<typename FlipGraph::IndexType, typename FlipGraph::InsideArrayRefType>>
      stack;
  std::vector<typename FlipGraph::InsideArrayRefType> edge_path;
  do {
    std::fill(visited_array.begin(), visited_array.end(), 0);
    path_improved = false;

    bool path_not_improved = true;
    for (IndexType v = IndexType{0}; v < instance.vertex_count(); v++) {
      auto outflow = instance.outflow(v);
      if (outflow == max_flow) {
        if (reset_every_time) {
          std::fill(visited_array.begin(), visited_array.end(), 0);
        }
        bool path_improved2 =
            improve_paths_stack(instance, v, outflow, stack, edge_path, visited_array);
        if (path_improved2) {
          path_improved = true;
          change_happening = true;
        } else {
          path_not_improved = false;
        }
      }
    }
    // if (max_flow_index <= 0) {
    if (path_not_improved) {
      max_flow--;
      if (max_flow == lower_bound) {
        return;
      }
      // std::cout << max_flow << std::endl;
    }  // else reset
    // }
  } while (path_improved);
}
}  // namespace dfs_naive
}  // namespace edge_orientation
}  // namespace rpo
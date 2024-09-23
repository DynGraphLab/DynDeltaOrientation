#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <utility>
#include <vector>

#include "edge_orientation/ds/smaller_graph.h"
#include "utils/logging.h"

namespace rpo {
namespace edge_orientation {
namespace bfs {

template <class FlipGraph>
std::enable_if_t<not rpo::edge_orientation::ds::is_smaller_type<FlipGraph>::value, bool>
bfs_up_to(
    FlipGraph& instance, typename FlipGraph::OutflowType max_flow,
    std::vector<std::pair<typename FlipGraph::InsideArrayRefType, typename FlipGraph::IndexType>>&
        prev_array) {
  TIMED_FUNC(timer);
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;

  const InsideArrayRefType not_visited_value =
      InsideArrayRefType{std::numeric_limits<typename InsideArrayRefType::ValueType>::max()};
  bool impr = false;
  size_t different = 0;
  size_t elements = 0;
  for (IndexType v{0}; v < instance.vertex_count(); v++) {
    elements += (instance.outflow(v) == max_flow);
  }
  if (elements == 0) {
    return false;
  }
  while (true) {
    std::fill(prev_array.begin(), prev_array.end(),
              std::make_pair<>(not_visited_value, IndexType{}));
    std::vector<std::pair<IndexType, typename IndexType::ValueType>>
        queue;  // queue with length //TODO remove height

    std::vector<IndexType> targets;
    for (IndexType v{0}; v < instance.vertex_count(); v++) {
      auto outflow = instance.outflow(v);
      if (outflow == max_flow) {
        prev_array[v] = {InsideArrayRefType{0}, IndexType{targets.size()}};
        queue.push_back({v, 0});
        targets.push_back(v);
      }
    }
    std::vector<bool> found_for(targets.size(), false);
    size_t ptr = 0;
    bool found = false;
    // std::cout << "Size:" << queue.size() << std::endl;
    //  std::vector<std::pair<IndexType, IndexType>> end_points;
    while (ptr < queue.size()) {
      auto [v, height] = queue[ptr++];
      if (found_for[prev_array[v].second]) {  // && height > search_untilheight) {
        prev_array[v] = {not_visited_value, IndexType{}};
        continue;  // break;
        // maybe reset prev info
      }
      auto out_v = instance.outflow(v);
      if (out_v < max_flow - 1) {
        found_for[prev_array[v].second] = true;
        found = true;
        impr = true;
        // end_points.push_back({v, targets[prev_array[v].second]});
        different++;
        auto w = v;
        auto target = targets[prev_array[v].second];
        while (w != target) {
          auto& [edge, path_target] = prev_array[w];
          // prev_array[w] = not_visited_value - 1;
          auto next = instance.nodeInEdge1(edge);
          if (next == w) {
            next = instance.nodeInEdge0(edge);
          }
          instance.flip_edge(edge);
          edge = not_visited_value;
          w = next;
        }
        if (std::accumulate(found_for.begin(), found_for.end(), 0UL) == found_for.size()) {
          break;
        }
      }
      auto [start_v, count_v] = instance.incEdges(v, 0);
      for (auto ix = start_v; ix < count_v; ix++) {
        const auto e = instance.flat_edge_info[ix];
        const auto w = instance.nodeInEdge1(e);
        if (prev_array[w].first == not_visited_value) {
          prev_array[w] = {e, prev_array[v].second};
          queue.push_back({w, height + 1});
        }
      }
      auto [start_u, count_u] = instance.incEdges(v, 1);

      for (auto ix = start_u; ix < count_u; ix++) {
        const auto e = instance.flat_edge_info[ix];
        const auto w = instance.nodeInEdge0(e);

        if (prev_array[w].first == not_visited_value) {
          prev_array[w] = {e, prev_array[v].second};
          queue.push_back({w, height + 1});
        }
      }
    }
    if (!found) {
      break;
    }
    // std::cout << "Found:" << end_points.size() << " " << search_untilheight << std::endl;
    // std::sort(end_points.begin(), end_points.end(),
    //          [](auto a, auto b) { return a.second < b.second; });
    // IndexType old_target = IndexType{std::numeric_limits<typename IndexType::ValueType>::max()};
    // for (auto [w, target] : end_points) {
    // if (old_target == target) {
    //   continue;
    // }
    // old_target = target;
    // different++;
    // while (w != target) {
    //   auto [edge, path_target] = prev_array[w];
    //   // prev_array[w] = not_visited_value - 1;
    //   auto next = std::get<1>(instance._edges[edge]);
    //   if (next == w) {
    //     next = std::get<0>(instance._edges[edge]);
    //   }
    //   instance.flip_edge(edge);
    //   w = next;
    // }
    //}
    if (different == elements) {
      break;
    }
  }
  // std::cout << max_flow << " Dif: " << different << " " << elements << std::endl;

  return elements == different;
}
template <class FlipGraph>
std::enable_if_t<rpo::edge_orientation::ds::is_smaller_type<FlipGraph>::value, bool>
bfs_up_to(
    FlipGraph& instance, typename FlipGraph::OutflowType max_flow,
    std::vector<std::pair<typename FlipGraph::InsideArrayRefType, typename FlipGraph::IndexType>>&
        prev_array) {
  TIMED_FUNC(timer);
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;

  const InsideArrayRefType not_visited_value =
      InsideArrayRefType{std::numeric_limits<typename InsideArrayRefType::ValueType>::max()};
  bool impr = false;
  size_t different = 0;
  size_t elements = 0;
  for (IndexType v{0}; v < instance.vertex_count(); v++) {
    elements += (instance.outflow(v) == max_flow);
  }
  if (elements == 0) {
    return false;
  }
  while (true) {
    std::fill(prev_array.begin(), prev_array.end(),
              std::make_pair<>(not_visited_value, IndexType{}));
    std::vector<std::pair<IndexType, typename IndexType::ValueType>>
        queue;  // queue with length //TODO remove height

    std::vector<IndexType> targets;
    for (IndexType v{0}; v < instance.vertex_count(); v++) {
      auto outflow = instance.outflow(v);
      if (outflow == max_flow) {
        prev_array[v] = {InsideArrayRefType{0}, IndexType{targets.size()}};
        queue.push_back({v, 0});
        targets.push_back(v);
      }
    }
    std::vector<bool> found_for(targets.size(), false);
    size_t ptr = 0;
    bool found = false;
    // std::cout << "Size:" << queue.size() << std::endl;
    //  std::vector<std::pair<IndexType, IndexType>> end_points;
    while (ptr < queue.size()) {
      auto [v, height] = queue[ptr++];
      if (found_for[prev_array[v].second]) {  // && height > search_untilheight) {
        prev_array[v] = {not_visited_value, IndexType{}};
        continue;  // break;
        // maybe reset prev info
      }
      auto out_v = instance.outflow(v);
      if (out_v < max_flow - 1) {
        found_for[prev_array[v].second] = true;
        found = true;
        impr = true;
        // end_points.push_back({v, targets[prev_array[v].second]});
        different++;
        auto w = v;
        auto target = targets[prev_array[v].second];
        while (w != target) {
          auto& [edge, path_target] = prev_array[w];
          // prev_array[w] = not_visited_value - 1;
          auto next = instance.flat_edge_info[edge];
          instance.flip_reverse_edge(w, edge);
          edge = not_visited_value;
          w = next;
        }
        if (std::accumulate(found_for.begin(), found_for.end(), 0UL) == found_for.size()) {
          break;
        }
      }
      const auto until_ = instance._nodes_offset[v] + InsideArrayRefType{instance.outflow(v)};
      for (auto e = instance._nodes_offset[v]; e < until_; e++) {
        const auto w = instance.flat_edge_info[e];
        if (prev_array[w].first == not_visited_value) {
          prev_array[w] = {instance._nodes_offset[w] +
                               InsideArrayRefType{instance.flat_edge_info_rel_reverse[e]},
                           prev_array[v].second};
          queue.push_back({w, height + 1});
        }
      }
    }
    if (!found) {
      break;
    }
    // std::cout << "Found:" << end_points.size() << " " << search_untilheight << std::endl;
    // std::sort(end_points.begin(), end_points.end(),
    //          [](auto a, auto b) { return a.second < b.second; });
    // IndexType old_target = IndexType{std::numeric_limits<typename IndexType::ValueType>::max()};
    // for (auto [w, target] : end_points) {
    // if (old_target == target) {
    //   continue;
    // }
    // old_target = target;
    // different++;
    // while (w != target) {
    //   auto [edge, path_target] = prev_array[w];
    //   // prev_array[w] = not_visited_value - 1;
    //   auto next = std::get<1>(instance._edges[edge]);
    //   if (next == w) {
    //     next = std::get<0>(instance._edges[edge]);
    //   }
    //   instance.flip_edge(edge);
    //   w = next;
    // }
    //}
    if (different == elements) {
      break;
    }
  }
  // std::cout << max_flow << " Dif: " << different << " " << elements << std::endl;

  return elements == different;
}
template <class FlipGraph>
void bfs_based_orientation(FlipGraph& instance) {
  TIMED_FUNC(timer);
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;
  using OutFlowType = typename FlipGraph::OutflowType;
  OutFlowType max_flow = instance.computeOutflows();
  typename FlipGraph::OutflowType lower_bound =
      std::ceil(instance.edge_count() / (double)instance.vertex_count());
  if (max_flow == lower_bound) {
    return;
  }
  const InsideArrayRefType not_visited_value =
      InsideArrayRefType{std::numeric_limits<InsideArrayRefType>::max()};
  std::vector<std::pair<typename FlipGraph::InsideArrayRefType, typename FlipGraph::IndexType>>
      prev_array(instance.vertex_count(), {not_visited_value, IndexType{}});

  while (bfs_up_to(instance, max_flow--, prev_array)) {
    // std::cout << max_flow << std::endl;
    if (max_flow == lower_bound) {
      break;
    }
  }
}
}  // namespace bfs

}  // namespace edge_orientation
}  // namespace rpo
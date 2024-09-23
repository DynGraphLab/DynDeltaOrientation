#pragma once

#include <cmath>

#include "edge_orientation/tools/bfs.h"
#include "edge_orientation/tools/dfs.h"

namespace rpo {
namespace edge_orientation {
namespace combined {
template <class FlipGraph>
void solve_by_combine(FlipGraph& instance, typename FlipGraph::OutflowType switch_to_bfs_count) {
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;
  TIMED_FUNC(tere);

  bool path_improved = false;
  bool change_happening = false;
  auto max_flow = instance.computeOutflows();
  std::vector<bool> visited_array(instance.vertex_count(), false);
  std::vector<std::pair<typename FlipGraph::IndexType, typename FlipGraph::InsideArrayRefType>>
      stack;
  std::vector<typename FlipGraph::InsideArrayRefType> edge_path;
  size_t numberOfImprovs = 0;
  size_t passes = 0;
  do {
    std::fill(visited_array.begin(), visited_array.end(), 0);
    path_improved = false;
    bool path_not_improved = true;
    for (IndexType v{0}; v < instance.vertex_count(); v++) {
      auto outflow = instance.outflow(v);
      if (outflow == max_flow) {
        bool path_improved2 = rpo::edge_orientation::dfs::improve_paths_stack(
            instance, v, outflow, stack, edge_path, visited_array);
        if (path_improved2) {
          path_improved = true;
          change_happening = true;
          numberOfImprovs++;
        } else {
          path_not_improved = false;
        }
      }
    }
    // if (max_flow_index <= 0) {
    if (path_not_improved) {
      max_flow--;
      // std::cout << max_flow << " " << numberOfImprovs << " " << passes << std::endl;
      if (numberOfImprovs > switch_to_bfs_count && passes > 4) {
        using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
        using IndexType = typename FlipGraph::IndexType;
        using OutFlowType = typename FlipGraph::OutflowType;

        const InsideArrayRefType not_visited_value =
            InsideArrayRefType{std::numeric_limits<typename InsideArrayRefType::ValueType>::max()};
        std::vector<
            std::pair<typename FlipGraph::InsideArrayRefType, typename FlipGraph::IndexType>>
            prev_array(instance.vertex_count(), {not_visited_value, IndexType{}});
        OutFlowType max_flow = instance.computeOutflows();
        while (bfs::bfs_up_to(instance, max_flow--, prev_array)) {
        }
        return;
      }
      numberOfImprovs = 0;
      passes = 0;

      // std::cout << max_flow << std::endl;
    } else {
      passes++;
    }
    // else reset
    // }
  } while (path_improved);
}
template <class FlipGraph>
void solve_by_dfs_combined_multiple(FlipGraph& instance, int multiple_count, bool finish,
                                    size_t eager_size) {
  using InsideArrayRefType = typename FlipGraph::InsideArrayRefType;
  using IndexType = typename FlipGraph::IndexType;
  TIMED_FUNC(tere);

  bool path_improved = false;
  auto max_flow = instance.computeOutflows();
  typename FlipGraph::OutflowType lower_bound =
      std::ceil(((double)instance.edge_count()) / ((double)instance.vertex_count()));
  if (max_flow == lower_bound) {
    return;
  }
  bool encountered_big = false;
  {
    std::vector<bool> visited_array(instance.vertex_count(), false);

    std::vector<std::pair<typename FlipGraph::IndexType, typename FlipGraph::InsideArrayRefType>>
        stack;
    std::vector<typename FlipGraph::InsideArrayRefType> edge_path;
    do {
      std::fill(visited_array.begin(), visited_array.end(), 0);
      path_improved = false;

      if (multiple_count + 1 > max_flow) {
        multiple_count = 1;
      }
      std::vector<std::vector<typename FlipGraph::IndexType>> buckets(multiple_count);

      auto target_flow = max_flow - multiple_count + 1;
      for (IndexType v{0}; v < instance.vertex_count(); v++) {
        auto outflow = instance.outflow(v);
        if (outflow >= target_flow) {
          buckets[max_flow - outflow].push_back(v);
        }
      }
      // for (auto bx = buckets.rbegin(); bx != buckets.rend(); bx++) {
      //   std::cout << "BUCeKET " << out_flow + bx - buckets.rbegin() << " " << bx->size() <<
      //   std::endl;
      // }
      if (buckets.back().size() >
          sqrt(instance.edge_count()) * (log10(instance.edge_count()) / log10(lower_bound))) {
        // std::cout << buckets.back().size() << " " << instance.vertex_count() << " "
        //           << instance.edge_count() << std::endl;
        multiple_count--;
        encountered_big = true;  // prevent recollection from this point on
        buckets.resize(buckets.size() - 1);
        if (multiple_count == 0) {
          break;
        }
      }
      auto out_flow = max_flow - multiple_count + 1;

      bool change_happening = false;

      bool singular_bucket = false;
      bool path_not_improved = true;
      for (auto bx = buckets.rbegin(); bx != buckets.rend(); bx++) {
        if (bx->size() < eager_size) {
          for (const auto& v : *bx) {
            while (instance.outflow(v) >= std::max((max_flow - multiple_count + 1), lower_bound) &&
                   rpo::edge_orientation::dfs::improve_paths_stack(
                       instance, v, instance.outflow(v), stack, edge_path, visited_array)) {
              singular_bucket = true;  // diferrent variable, because requires refresh
            }
          }

        } else {
          for (const auto& v : *bx) {
            bool path_improved2 = rpo::edge_orientation::dfs::improve_paths_stack(
                instance, v, instance.outflow(v), stack, edge_path, visited_array);
            if (path_improved2) {
              path_improved = true && out_flow == max_flow;
              change_happening = true;
            } else if (out_flow == max_flow) {
              path_not_improved = false;
            }
          }
        }
        out_flow++;
      }
      // if (max_flow_index <= 0) {
      max_flow = instance.computeOutflows();
      if (max_flow == lower_bound) {
        return;
      }
      if (singular_bucket) {
        path_improved = true;
      } else if (path_not_improved) {
        // max_flow--;
        if (encountered_big) {
          multiple_count--;
          if (multiple_count == 0) {
            break;
          }
        }
      } else if (multiple_count <= 2) {  // if two buckets only left exit
        break;
      }
      // }
    } while (path_improved);  // exit on top
  }
  max_flow = instance.computeOutflows();
  if (max_flow == lower_bound) {
    return;
  }
  if (finish) {
    if (max_flow <= 9 || encountered_big) {
      rpo::edge_orientation::dfs::solve_by_dfs(instance);
      return;
    } else {
      const InsideArrayRefType not_visited_value =
          InsideArrayRefType{std::numeric_limits<typename InsideArrayRefType::ValueType>::max()};
      std::vector<std::pair<typename FlipGraph::InsideArrayRefType, typename FlipGraph::IndexType>>
          prev_array(instance.vertex_count(), {not_visited_value, IndexType{}});
      while (bfs::bfs_up_to(instance, max_flow--, prev_array)) {
        if (max_flow == lower_bound) {
          return;
        }
      }
    }
  }
}
}  // namespace combined
}  // namespace edge_orientation
}  // namespace rpo
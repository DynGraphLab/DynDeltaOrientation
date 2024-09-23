#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include "ds/addressable_priority_queue.h"
#include "utils/logging.h"

namespace rpo {
namespace edge_orientation {
namespace two_approx {
template <class SimpleSetGraph>
std::unique_ptr<SimpleSetGraph> remove_up_to(const SimpleSetGraph& instance,
                                             typename SimpleSetGraph::NodeType two_approx) {
  TIMED_FUNC(timer);
  std::vector<typename SimpleSetGraph::NodeType> task;
  std::vector<typename SimpleSetGraph::NodeType> out_degree(instance.vertex_count());
  std::vector<bool> hasEnteredQueue(out_degree.size(), false);
  auto nVertices = instance.vertex_count();
  {
    typename SimpleSetGraph::NodeType v = 0;
    for (auto& neighbors : instance.entries) {
      out_degree[v] = neighbors.size();
      if (out_degree[v] <= two_approx) {
        task.push_back(v);
        hasEnteredQueue[v] = true;
        nVertices--;
      }
      v++;
    }
  }
  while (task.size() != 0) {
    auto v = task.front();
    task.front() = task.back();
    task.resize(task.size() - 1);
    for (auto w : instance.entries[v]) {
      out_degree[w]--;

      if (out_degree[w] <= two_approx && !hasEnteredQueue[w]) {
        hasEnteredQueue[w] = true;
        nVertices--;
        task.push_back(w);
      }
    }
  }

  auto result = std::make_unique<SimpleSetGraph>(0, nVertices);
  typename SimpleSetGraph::NodeType name = 0;
  std::vector<typename SimpleSetGraph::NodeType> names(instance.vertex_count());
  for (typename SimpleSetGraph::NodeType v = 0; v < instance.vertex_count(); v++) {
    if (!hasEnteredQueue[v]) {
      names[v] = name++;
    }
  }
  {
    typename SimpleSetGraph::NodeType v = 0;
    for (auto& neighbors : instance.entries) {
      if (!hasEnteredQueue[v]) {
        for (auto u : neighbors) {
          if (!hasEnteredQueue[u]) {
            result->addEdge(names[v], names[u], 1);
          }
        }
      }
      v++;
    }
  }
  return result;
}
template <class SimpleSetGraph>
typename SimpleSetGraph::NodeType compute_two_approx(SimpleSetGraph& instance) {
  TIMED_FUNC(timer);
  typename SimpleSetGraph::NodeType maxDeg = 0;
  typename SimpleSetGraph::NodeType minDegUnequalZero = instance.vertex_count();

  for (auto& neighbors : instance.entries) {
    maxDeg = std::max(maxDeg, (typename SimpleSetGraph::NodeType)neighbors.size());
    if (neighbors.size() != 0) {
      minDegUnequalZero =
          std::min(minDegUnequalZero, (typename SimpleSetGraph::NodeType)neighbors.size());
    }
  }
  std::vector<typename SimpleSetGraph::NodeType> out_degree(instance.vertex_count());
  std::vector<typename SimpleSetGraph::NodeType> orientedTowardsMe(instance.vertex_count(), 0);

  std::vector<std::vector<typename SimpleSetGraph::NodeType>> degreeTables(maxDeg + 1);
  for (int i = 1; i < std::min((int)maxDeg, 4); i++) {
    degreeTables.reserve(instance.vertex_count());
  }
  std::vector<size_t> links;
  auto numVertices = instance.vertex_count();
  auto numEdges = instance.edge_count();
  {
    typename SimpleSetGraph::NodeType v = 0;
    for (auto& neighbors : instance.entries) {
      if (neighbors.size() == 0) {
        numVertices--;
      }
      out_degree[v] = neighbors.size();
      degreeTables[out_degree[v]].push_back(v);
      v++;
    }
  }
  rpo::ds::FullAdressableDecreasingPriorityQueue<typename SimpleSetGraph::NodeType,
                                                 typename SimpleSetGraph::NodeType>
      queue(out_degree);
  while (out_degree[queue.lowest_level_item()] == 0) {
    queue.pop_back();
  }

  double maxRatio = numEdges / (double)numVertices;
  double previousRatio = numEdges / (double)numVertices;
  {
    TIMED_SCOPE(timer2, "approx_comp");
    AGG_TIMER(explore, "exploration");
    AGG_TIMER(pushing, "pushing");

    size_t improved = 0;
    double _last_ratio = 0;
    while (!queue.empty()) {
      auto v = queue.lowest_level_item();
      double rat = numEdges / (double)numVertices;
      if (rat > maxRatio) {
        improved++;
        maxRatio = rat;
      }
      previousRatio = rat;

      orientedTowardsMe[v] = out_degree[v];
      {
        // AGG_TIMER_PART(explore);
        for (int neighb : instance.entries[v]) {
          if (out_degree[neighb] == 0) {
            continue;
          }
          //  AGG_TIMER_PART(pushing);

          // degreeTables[out_degree[neighb]][links[neighb]] =
          // degreeTables[out_degree[neighb]].back();
          // links[degreeTables[out_degree[neighb]][links[neighb]]] = links[neighb];
          // degreeTables[out_degree[neighb]].pop_back();

          // out_degree[neighb]--;
          queue.push_down(neighb);
          if (out_degree[neighb] != 0) {
            // links[neighb] = degreeTables[out_degree[neighb]].size();
            //  degreeTables[out_degree[neighb]].emplace_back(neighb);
          } else {
            numVertices--;
          }
        }
      }
      numVertices--;
      numEdges -= out_degree[v];
      while (out_degree[v] != 0) {
        queue.push_down(v);
      };
    }
  }
  typename SimpleSetGraph::NodeType mdeg = 0;
  for (auto m : orientedTowardsMe) {
    mdeg = std::max(mdeg, m);
  }
  return std::max(((typename SimpleSetGraph::NodeType)std::floor(2 * maxRatio)), mdeg);
}
}  // namespace two_approx
}  // namespace edge_orientation
}  // namespace rpo
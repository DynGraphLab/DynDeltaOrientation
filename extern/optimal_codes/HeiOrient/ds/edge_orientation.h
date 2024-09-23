#pragma once

#include <numeric>
#include <vector>

#include "utils/range.h"

namespace rpo {
namespace ds {
namespace {
using rpo::utils::Range;
}
template <typename Graph>
class EdgeOrientation {
  std::vector<bool> orientations;
  std::unique_ptr<Graph> _graph;
  std::vector<size_t> outdegree;

 public:
  static constexpr absl::string_view ds_name = "edge_orientation";
  EdgeOrientation(std::unique_ptr<Graph> graph) : _graph(std::move(graph)) {
    orientations.resize(_graph->edge_count(), false);
    outdegree.resize(_graph->vertex_count(), 0);
    for (auto n : _graph->nodes()) {
      auto inc = _graph->edgesFrom(n);
      outdegree[n] = std::transform_reduce(inc.begin(), inc.end(), 0L, std::plus{}, [&](auto e) {
        return orientations[e] != (_graph->edge(e)[1] == n);
      });
    }
  }
  bool orientation(const typename Graph::EdgeType e) const { return orientations[e]; }
  void SetOrientation(const typename Graph::EdgeType e, bool val) {
    if (val == orientations[e]) {
      return;
    }
    orientations[e] = val;
    if (val) {
      outdegree[_graph->edge(e)[0]]++;
      outdegree[_graph->edge(e)[1]]--;
    } else {
      outdegree[_graph->edge(e)[0]]--;
      outdegree[_graph->edge(e)[1]]++;
    }
  }
  auto outGoingOrientationAt(const typename Graph::NodeType n) { return outdegree[n]; }
  auto size() {
    auto maxOut = outGoingOrientationAt(0);
    for (auto i : _graph->nodes()) {
      maxOut = std::max(maxOut, outGoingOrientationAt(i));
    }
    return maxOut;
  }
  auto free_edges_size() const { return 0; }

  auto weight() { return size(); }
  auto quality() { return 0; }
  const Graph& instance() const { return *_graph; }
  bool valid() const { return true; }

  using InstanceType = Graph;
};
}  // namespace ds
}  // namespace rpo
#pragma once
#include <array>
#include <set>
#include <vector>

#include "utils/logging.h"
#include "utils/range.h"

namespace rpo {

namespace ds {
namespace {
using rpo::utils::FullRange;
using rpo::utils::Range;
using rpo::utils::SimpleIterator;
}  // namespace

// TODO Decorators
template <typename NType = size_t, typename WType = double>
class UndirectedGraph {
  std::vector<std::vector<size_t>> _adj_array;                 // note these are the edges
  std::vector<std::pair<std::array<NType, 2>, WType>> _edges;  // there is some indirection

 public:
  using EdgeType = size_t;
  using NodeType = NType;
  using WeightType = WType;
  UndirectedGraph(size_t num_edges, size_t vertex_count) : _adj_array(vertex_count) {
    _edges.reserve(num_edges);
  }
  void addEdge(NodeType u, NodeType v, WeightType weight = 1) {
    if (u < v) {
      // assert(u < _adj_array.size());
      // assert(v < _adj_array.size());
      addEdge({u, v}, weight);
    }
  }
  void addEdge(std::array<NodeType, 2> edge, WType weight = 1) {
    // assert(edge[0] < _adj_array.size());
    // assert(edge[1] < _adj_array.size());
    _edges.push_back(std::make_pair(edge, weight));
    size_t e = _edges.size() - 1;
    if (e > ((EdgeType)std::numeric_limits<int>::max())) {
      // std::cout << "WARN int_overflow: " << e << std::endl;
    }
    _adj_array[edge[0]].push_back(e);
    _adj_array[edge[1]].push_back(e);
  }
  void shrink_to_size() {}
  auto edges() const {
    return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(_edges.size()));
  }
  auto nodes() const {
    return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(_adj_array.size()));
  }
  auto edges_direct() const { return FullRange(_edges); }
  auto edge(const EdgeType id) const {
    // assert(id < _edges.size());
    return _edges[id].first;
  }
  auto pins(const EdgeType e) const { return edge(e); }
  auto incidentEdges(NodeType n) const { return edgesFrom(n); }
  auto edgesFrom(NodeType n) const {
    assert(n < _adj_array.size());
    return FullRange(_adj_array[n]);
  }
  auto edge_count() const { return _edges.size(); }
  auto vertex_count() const { return _adj_array.size(); }
  auto currentNumNodes() const { return _adj_array.size(); }
  auto currentNumEdges() const { return _edges.size(); }
  void setNodeWeight(NodeType, WeightType w) {}
  void setEdgeWeight(EdgeType id, WeightType w) { _edges[id].second = w; }
  WeightType edgeWeight(size_t edge_id) { return _edges[edge_id].second; }
  WeightType nodeWeight(NodeType) { return 1; }

  size_t edgeSize(int e) { return 2; }
  void finish() {}
  auto maxDegree() const {
    auto deg = _adj_array[0].size();
    for (auto& ar : _adj_array) {
      deg = std::max(deg, ar.size());
    }
    return deg;
  }
  bool validate() {
    std::set<std::pair<NodeType, NodeType>> map;
    for (auto [p, _] : _edges) {
      std::pair<NodeType, NodeType> as_set = {std::min(p[0], p[1]), std::max(p[0], p[1])};
      if (map.find(as_set) != map.end()) {
        return false;
      }
      map.insert(as_set);
    }
    return true;
  }
  auto nodeDegree(NodeType n) const { return _adj_array[n].size(); }
};

}  // namespace ds
}  // namespace rpo
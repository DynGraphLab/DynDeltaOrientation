#pragma once
#include <memory>
#include <numeric>
#include <vector>

#include "absl/strings/string_view.h"
#include "utils/logging.h"
#include "utils/range.h"

namespace rpo {
namespace ds {

template <class Graph>
class EmptyProblem {
 public:
  std::unique_ptr<Graph> _g;
  size_t _size = 0;
  int _weight = 0;
  Graph& instance() { return *_g; }
  using Graph_t = Graph;
  using NodeID_t = typename Graph::NodeType;
  using EdgeID_t = typename Graph::EdgeType;
  using NodeType = typename Graph::NodeType;
  using EdgeType = typename Graph::EdgeType;
  using WeightType = typename Graph::WeightType;

  EmptyProblem(std::unique_ptr<Graph> g) : _g(std::move(g)) {}

  bool valid() { return true; }

  size_t size() const { return _size; }

  int weight() const { return _weight; }

  int free_edges_size() { return 0; }
  double quality() const { return 0; }

  void save(std::string filename) {}
  const static constexpr std::string_view ds_name = "empty_problem";
  void setWeight(int w) { _weight = w; }
  void setSize(size_t w) { _size = w; }
};
template <typename DataType, typename ET = size_t>
struct SimpleSetGraph {
  using NodeID_t = DataType;
  using EdgeID_t = ET;
  using NodeType = DataType;
  using EdgeType = ET;
  using WeightType = DataType;
  std::vector<std::vector<DataType>> entries;
  size_t edges_, nodes_;
  SimpleSetGraph(size_t e, size_t n) : entries(n) {
    edges_ = 0;
    nodes_ = n;
  }
  SimpleSetGraph(const SimpleSetGraph&) = delete;
  SimpleSetGraph(SimpleSetGraph&&) = delete;
  SimpleSetGraph& operator=(const SimpleSetGraph&) = delete;
  SimpleSetGraph& operator=(SimpleSetGraph&&) = delete;
  ~SimpleSetGraph() = default;
  void addEdge(DataType u, DataType v, DataType w) {
    if (u < v) {
      entries[u].push_back(v);
      entries[v].push_back(u);
      edges_++;
    }
  }
  void shrink_to_size() {
    size_t count = 0;
    for (auto& neighbors : entries) {
      std::sort(neighbors.begin(), neighbors.end());
      count += neighbors.size();
    }
    edges_ = count / 2;
  }
  void finish() { shrink_to_size(); }
  void setNodeWeight(NodeType, WeightType w) {}
  void setEdgeWeight(EdgeType, WeightType w) {}
  auto currentNumEdges() { return edges_; }
  auto currentNumNodes() { return nodes_; }
  auto vertex_count() const { return entries.size(); }
  auto edge_count() const { return edges_; }
  auto edges() {
    using rpo::utils::SimpleIterator;
    return rpo::utils::Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(edges_));
  }
  auto nodes() {
    using rpo::utils::SimpleIterator;
    return rpo::utils::Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(nodes_));
  }
  DataType edgeSize(EdgeType e) { return 2; }
  // removes v from u
  void removeEdge(NodeType u, NodeType v) {
    for (auto pointer = entries[u].begin(); pointer != entries[u].end(); pointer++) {
      if (*pointer == v) {
        *pointer = entries[u].back();
        entries[u].resize(entries[u].size() - 1);
        return;
      }
    }
  }
};
using StandardSimpleSetGraph = SimpleSetGraph<uint32_t>;
}  // namespace ds
}  // namespace rpo
#pragma once

#include <array>
#include <tuple>
#include <vector>

#include "absl/strings/string_view.h"
#include "utils/logging.h"
#include "utils/range.h"
#include "utils/strong_type.h"

namespace rpo {
namespace edge_orientation {
namespace ds {
namespace {
using rpo::utils::Range;
using rpo::utils::SimpleIterator;
}  // namespace

template <typename IT = int, typename IARF = size_t>
struct SepFlipGraph {
  class InsideArrayRefTag;
  using InsideArrayRefType = StrongType<IARF, InsideArrayRefTag>;
  using IndexType = StrongType<IT, InsideArrayRefTag>;

  using OutflowType = IT;
  using EdgeType = IndexType;
  using NodeType = IndexType;
  using WeightType = IndexType;

  // std::vector<bool> flows; // size m, true means flow to u, if edge is u-v
  static constexpr int DIRECTION = 2;
  using EdgeInfo = std::tuple<bool, bool, InsideArrayRefType,
                              InsideArrayRefType>;  // rindex
  std::vector<EdgeInfo> _edges;
  std::array<std::vector<IndexType>, 2> _edges_;

  // in position 0: flow==true means flow towards it
  // in position 1: flow==false means flow towards it
  // tuple
  // 0 --> outflow TODO refactor
  // 1 --> neighboring info
  // 2 --> visited
  using NodeInfo =
      // IndexType start offset of this partiuclar node
      std::array<std::pair<InsideArrayRefType, InsideArrayRefType>, 2>;
  std::vector<InsideArrayRefType> flat_edge_info;
  std::vector<NodeInfo> _nodes;         // n+1
  std::vector<OutflowType> _nodes_out;  // n+1

  auto edges() const {
    return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(_edges.size()));
  }
  auto nodes() const {
    return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(_nodes.size()));
  }
  double quality() { return 0; }
  auto edge_count() const { return _edges.size(); }
  size_t vertex_count() const { return _nodes.size(); }
  auto currentNumNodes() const { return _nodes.size(); }
  auto currentNumEdges() const { return _edges.size(); }
  auto free_edges_size() { return _edges.size(); }
  auto size() { return computeOutflows(); }
  auto weight() { return size(); }
  // returns maxOutflow
  struct InstanceImpl {
    IT _n;
    IARF _e;
    InstanceImpl(IT n, IARF e) : _n(n), _e(e) {}
    auto currentNumNodes() const { return _n; }
    auto currentNumEdges() const { return _e; }
  };
  InstanceImpl instance_;
  InstanceImpl& instance() { return instance_; }

  auto computeOutflows() {
    OutflowType max_f{0};
    for (auto o : _nodes_out) {
      if (max_f <= o) {
        max_f = o;
      }
    }
    return max_f;
  }
  auto computeNumberMaxFlow() {
    OutflowType max_f{0};
    OutflowType max_flow_index = OutflowType{0};
    for (auto& [o, _] : _nodes) {
      if (max_f <= o) {
        max_flow_index *= (max_f == o);
        max_f = o;
        max_flow_index++;
      }
    }
    return max_flow_index;
  }
  inline OutflowType outflow(IndexType v) const { return _nodes_out[v]; }
  inline auto& incEdges(IndexType v, int index) { return _nodes[v][index]; }
  inline IndexType nodeInEdge0(InsideArrayRefType e) const { return _edges_[0][e]; }
  inline IndexType nodeInEdge1(InsideArrayRefType e) const { return _edges_[1][e]; }

  inline void flip_edge(InsideArrayRefType e) {
    auto& [flow, settled_edge, ru, rv] = _edges[e];
    auto u = _edges_[0][e];
    auto v = _edges_[1][e];

    auto& outflow_u = _nodes_out[u];
    auto& outflow_v = _nodes_out[v];

    auto& incE_u = _nodes[u];
    auto& incE_v = _nodes[v];

    auto max_fl = std::max(outflow_u, outflow_v);
    if (!flow) {
      outflow_u++;
      outflow_v--;
      {
        auto old_u = flat_edge_info[incE_u[0].second];
        // move in u
        flat_edge_info[ru] = old_u;
        // update ru of old_u
        std::get<2>(_edges[old_u]) = ru;
        ru = incE_u[0].second;
        flat_edge_info[ru] = e;
        incE_u[0].second++;
      }
      // update in v
      incE_v[1].second--;
      auto old_v = flat_edge_info[incE_v[1].second];
      flat_edge_info[rv] = old_v;
      // update ru of old_u
      std::get<3>(_edges[old_v]) = rv;
      rv = incE_v[1].second;
      flat_edge_info[rv] = e;
    } else {
      outflow_u--;
      outflow_v++;
      {
        auto old_v = flat_edge_info[incE_v[1].second];
        // move in u
        flat_edge_info[rv] = old_v;
        // update ru of old_u
        std::get<3>(_edges[old_v]) = rv;
        rv = incE_v[1].second;
        flat_edge_info[rv] = e;
        incE_v[1].second++;
      }

      // update in v
      incE_u[0].second--;
      auto old_u = flat_edge_info[incE_u[0].second];
      flat_edge_info[ru] = old_u;
      // update ru of old_u
      std::get<2>(_edges[old_u]) = ru;
      ru = incE_u[0].second;
      flat_edge_info[ru] = e;
    }
    flow = !flow;
    assert(std::max(outflow_u, outflow_v) <= max_fl);
  }

  static constexpr absl::string_view ds_name = "sep_flip_graph";
  void finish() {}
  void resort_fast() {
    TIMED_FUNC(timer);
    InsideArrayRefType edge{0};
    for (auto [flow, fa, ru, rv] : _edges) {
      auto u = _edges_[0][edge];
      auto v = _edges_[1][edge];

      if (((flow & (_nodes_out[u] - 1 > _nodes_out[v])) |
           (!flow & (_nodes_out[u] < _nodes_out[v] - 1)))) {
        flip_edge(edge);
      }
      edge++;
    }
  }
  SepFlipGraph(const SepFlipGraph&) = delete;
  SepFlipGraph(SepFlipGraph&&) = delete;
  SepFlipGraph& operator=(const SepFlipGraph&) = delete;
  SepFlipGraph& operator=(SepFlipGraph&&) = delete;
  template <class SimpleSetGraph>
  SepFlipGraph(const SimpleSetGraph& set_graph)
      : _edges(set_graph.edges_),
        flat_edge_info(2UL * set_graph.edges_),
        _nodes(set_graph.nodes_),
        _nodes_out(set_graph.nodes_, 0),
        instance_(set_graph.nodes_, set_graph.edges_) {
    {
      TIMED_SCOPE(timer, "conversion");

      // setup graph
      IndexType u{0};
      InsideArrayRefType old_of{0};
      for (const auto& out_s : set_graph.entries) {
        _nodes[u][0].first = old_of;
        _nodes[u][0].second = old_of;
        old_of += out_s.size();
        _nodes[u][1].first = old_of;
        _nodes[u][1].second = old_of;
        u++;
      }
      _edges.resize(old_of / 2L);
      _edges_[0].resize(old_of / 2L);
      _edges_[1].resize(old_of / 2L);

      flat_edge_info.resize(old_of);
      u = IndexType{0};
      InsideArrayRefType edge{0};
      for (const auto& out_s : set_graph.entries) {
        for (auto v : out_s) {
          if (v > u) {
            break;
          }
          _nodes_out[v]++;
          flat_edge_info[_nodes[v][0].second] = edge;
          flat_edge_info[--(_nodes[u][1].first)] = edge;
          _nodes[u][1].second--;
          _edges_[0][edge] = IndexType{v};
          _edges_[1][edge] = IndexType{u};
          _edges[edge] = std::make_tuple(true, false, _nodes[v][0].second++, _nodes[u][1].first);
          edge++;
        }
        u++;
      }
    }
  }

  void setNodeWeight(IndexType, WeightType) {}
  void setEdgeWeight(IndexType, WeightType) {}
  int edgeSize(IndexType e) { return 2; }

  bool orientation(size_t edge) { return std::get<DIRECTION>(_edges[edge]); }

  bool valid() { return true; }

  void shrink_to_size() { throw; }
};
#ifdef AVX_ENABLED_DEFINED
using StandardSepFlipGraph = SepFlipGraph<long, long>;
#else
using StandardSepFlipGraph = SepFlipGraph<unsigned int, size_t>;
#endif
}  // namespace ds
}  // namespace edge_orientation
}  // namespace rpo
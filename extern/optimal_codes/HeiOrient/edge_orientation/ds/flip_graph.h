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
struct FlipGraph {
  class InsideArrayRefTag;
  using InsideArrayRefType = StrongType<IARF, InsideArrayRefTag>;
  using IndexType = StrongType<IT, InsideArrayRefTag>;

  using OutflowType = IT;
  using EdgeType = IndexType;
  using NodeType = IndexType;
  using WeightType = IndexType;

  // std::vector<bool> flows; // size m, true means flow to u, if edge is u-v
  static constexpr int DIRECTION = 2;
  using EdgeInfo = std::tuple<IndexType, IndexType, bool, bool, InsideArrayRefType,
                              InsideArrayRefType>;  // rindex
  std::vector<EdgeInfo> _edges;

  // in position 0: flow==true means flow towards it
  // in position 1: flow==false means flow towards it
  // tuple
  // 0 --> outflow TODO refactor
  // 1 --> neighboring info
  // 2 --> visited
  using NodeInfo = std::tuple<OutflowType,
                              // IndexType start offset of this partiuclar node
                              std::array<std::pair<InsideArrayRefType, InsideArrayRefType>, 2>>;
  std::vector<InsideArrayRefType> flat_edge_info;
  std::vector<NodeInfo> _nodes;  // n+1

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
    for (auto& [o, _] : _nodes) {
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
  inline OutflowType outflow(IndexType v) const { return std::get<0>(_nodes[v]); }
  inline auto& incEdges(IndexType v, int index) { return std::get<1>(_nodes[v])[index]; }

  inline IndexType nodeInEdge0(InsideArrayRefType e) const { return std::get<0>(_edges[e]); }
  inline IndexType nodeInEdge1(InsideArrayRefType e) const { return std::get<1>(_edges[e]); }

  inline void flip_edge(InsideArrayRefType e) {
    auto& [u, v, flow, _already, ru, rv] = _edges[e];
    // u
    // std::cout << u << " " << v << " flipped " << e << std::endl;
    auto& [outflow_u, incE_u] = _nodes[u];
    auto& [outflow_v, incE_v] = _nodes[v];

    auto max_fl = std::max(outflow_u, outflow_v);
    // if (_already) {
    //   std::cerr << e << " already turned" << std::endl;
    // }
    // _already = true;
    if (!flow) {
      outflow_u++;
      outflow_v--;
      {
        auto old_u = flat_edge_info[incE_u[0].second];
        // move in u
        flat_edge_info[ru] = old_u;
        // update ru of old_u
        std::get<4>(_edges[old_u]) = ru;
        ru = incE_u[0].second;
        flat_edge_info[ru] = e;
        incE_u[0].second++;
      }
      // update in v
      incE_v[1].second--;
      auto old_v = flat_edge_info[incE_v[1].second];
      flat_edge_info[rv] = old_v;
      // update ru of old_u
      std::get<5>(_edges[old_v]) = rv;
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
        std::get<5>(_edges[old_v]) = rv;
        rv = incE_v[1].second;
        flat_edge_info[rv] = e;
        incE_v[1].second++;
      }

      // update in v
      incE_u[0].second--;
      auto old_u = flat_edge_info[incE_u[0].second];
      flat_edge_info[ru] = old_u;
      // update ru of old_u
      std::get<4>(_edges[old_u]) = ru;
      ru = incE_u[0].second;
      flat_edge_info[ru] = e;
    }
    flow = !flow;
    assert(std::max(outflow_u, outflow_v) <= max_fl);
  }

  static constexpr absl::string_view ds_name = "flip_graph";
  void finish() {}
  void resort_fast() {
    TIMED_FUNC(timer);
    InsideArrayRefType edge{0};
    for (auto [u, v, flow, fa, ru, rv] : _edges) {
      if (((flow & (std::get<0>(_nodes[u]) - 1 > std::get<0>(_nodes[v]))) |
           (!flow & (std::get<0>(_nodes[u]) < std::get<0>(_nodes[v]) - 1)))) {
        flip_edge(edge);
      }
      edge++;
    }
  }
  FlipGraph(const FlipGraph&) = delete;
  FlipGraph(FlipGraph&&) = delete;
  FlipGraph& operator=(const FlipGraph&) = delete;
  FlipGraph& operator=(FlipGraph&&) = delete;
  template <class SimpleSetGraph>
  FlipGraph(const SimpleSetGraph& set_graph)
      : _edges(set_graph.edges_),
        flat_edge_info(2UL * set_graph.edges_),
        _nodes(set_graph.nodes_),
        instance_(set_graph.nodes_, set_graph.edges_) {
    {
      TIMED_SCOPE(timer, "conversion");

      // setup graph
      IndexType u{0};
      InsideArrayRefType old_of{0};
      for (const auto& out_s : set_graph.entries) {
        std::get<1>(_nodes[u])[0].first = old_of;
        std::get<1>(_nodes[u])[0].second = old_of;
        old_of += out_s.size();
        std::get<1>(_nodes[u])[1].first = old_of;
        std::get<1>(_nodes[u])[1].second = old_of;
        u++;
      }
      _edges.resize(old_of / 2L);

      flat_edge_info.resize(old_of);
      u = IndexType{0};
      InsideArrayRefType edge{0};
      for (const auto& out_s : set_graph.entries) {
        for (auto v : out_s) {
          if (v > u) {
            break;
          }
          std::get<0>(_nodes[v])++;
          flat_edge_info[std::get<1>(_nodes[v])[0].second] = edge;
          flat_edge_info[--(std::get<1>(_nodes[u])[1].first)] = edge;
          std::get<1>(_nodes[u])[1].second--;

          _edges[edge] =
              std::make_tuple(IndexType{v}, IndexType{u}, true, false,
                              std::get<1>(_nodes[v])[0].second++, std::get<1>(_nodes[u])[1].first);
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
using StandardFlipGraph = FlipGraph<unsigned int>;
}  // namespace ds
}  // namespace edge_orientation
}  // namespace rpo
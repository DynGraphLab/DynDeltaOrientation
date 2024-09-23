#pragma once

#include <array>
#include <cassert>
#include <numeric>
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
struct SmallerGraph {
  class InsideArrayRefTag;
  using InsideArrayRefType = StrongType<IARF, InsideArrayRefTag>;
  using IndexType = StrongType<IT, InsideArrayRefTag>;

  using OutflowType = IT;
  using EdgeType = IndexType;
  using NodeType = IndexType;
  using WeightType = IndexType;

  std::vector<IndexType> flat_edge_info;              // 2*m
  std::vector<IndexType> flat_edge_info_rel_reverse;  // 2*m

  std::vector<InsideArrayRefType> _nodes_offset;
  std::vector<OutflowType> _nodes_out;

  auto edges() const { return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(0)); }
  auto nodes() const {
    return Range<SimpleIterator>(SimpleIterator(0), SimpleIterator(_nodes_out.size()));
  }
  double quality() { return 0; }
  auto edge_count() const { return flat_edge_info.size() / 2; }
  size_t vertex_count() const { return _nodes_out.size(); }
  auto currentNumNodes() const { return _nodes_out.size(); }
  auto currentNumEdges() const { return flat_edge_info.size() / 2; }
  auto free_edges_size() { return flat_edge_info.size() / 2; }
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
  inline OutflowType outflow(IndexType v) const { return _nodes_out[v]; }

  inline void flip_reverse_edge(IndexType u, InsideArrayRefType e) {
    auto v = flat_edge_info[e];
    auto rev_rel = flat_edge_info_rel_reverse[e];
    flip_active_edge(v, _nodes_offset[v] + InsideArrayRefType{rev_rel});
  }
  /// Flips the edge oriented u --> flat_edge_info[e]
  inline void flip_active_edge(IndexType u, InsideArrayRefType e) {
    // checkall();
    assert(u == _nodes_offset.size() - 1 || e < _nodes_offset[u + 1]);
    assert(e >= _nodes_offset[u]);
    assert(e - _nodes_offset[u] < _nodes_out[u]);
    // edge oriented u-> v
    auto v = flat_edge_info[e];
    auto in_v_offset = flat_edge_info_rel_reverse[e];
    assert(in_v_offset >= _nodes_out[v]);

    auto& outflow_u = _nodes_out[u];
    assert(outflow_u > 0);
    auto& outflow_v = _nodes_out[v];

    // move v  in u part to end
    InsideArrayRefType last_in_u_array_active =
        _nodes_offset[u] + InsideArrayRefType{outflow_u - 1};
    IndexType last_in_u_array_target = flat_edge_info[last_in_u_array_active];
    IndexType last_in_u_array_target_reverse = flat_edge_info_rel_reverse[last_in_u_array_active];
    assert(flat_edge_info[_nodes_offset[last_in_u_array_target] +
                          InsideArrayRefType{last_in_u_array_target_reverse}] == u);
    flat_edge_info[e] = last_in_u_array_target;
    flat_edge_info_rel_reverse[e] = last_in_u_array_target_reverse;
    if (e != last_in_u_array_active) {
      flat_edge_info_rel_reverse[_nodes_offset[last_in_u_array_target] +
                                 InsideArrayRefType{last_in_u_array_target_reverse}] =
          IndexType{e - _nodes_offset[u]};
    }

    auto rev_e = _nodes_offset[v] + in_v_offset;
    assert(rev_e < flat_edge_info.size());
    assert(flat_edge_info[rev_e] == u);
    assert(flat_edge_info_rel_reverse[rev_e] == IndexType{e - _nodes_offset[u]});
    assert(rev_e >= _nodes_offset[v] + _nodes_out[v]);
    // now write
    assert(in_v_offset >= _nodes_out[v]);
    InsideArrayRefType last_in_v_inactive = _nodes_offset[v] + InsideArrayRefType{outflow_v};
    IndexType last_in_v_target = flat_edge_info[last_in_v_inactive];
    IndexType last_in_v_target_reverse = flat_edge_info_rel_reverse[last_in_v_inactive];

    flat_edge_info[rev_e] = last_in_v_target;
    flat_edge_info_rel_reverse[rev_e] = last_in_v_target_reverse;
    if (rev_e != last_in_v_inactive) {
      flat_edge_info_rel_reverse[_nodes_offset[last_in_v_target] +
                                 InsideArrayRefType{last_in_v_target_reverse}] = in_v_offset;
    }
    // now write u
    flat_edge_info[last_in_v_inactive] = u;
    flat_edge_info_rel_reverse[last_in_v_inactive] = IndexType{outflow_u - 1};

    // now write v into the last position
    flat_edge_info[last_in_u_array_active] = v;
    flat_edge_info_rel_reverse[last_in_u_array_active] = IndexType{outflow_v};
    assert(last_in_u_array_target == flat_edge_info[e]);
    outflow_v++;
    outflow_u--;
    // checkall();
  }

  static constexpr absl::string_view ds_name = "small_flip_graph";
  void finish() {}
  void resort_fast() {
    TIMED_FUNC(timer);
    IndexType start{_nodes_offset.size() - 1};
    for (IndexType k{0}; k < start; k++) {
      auto u = k;

      auto outflow_u = _nodes_out[u];
      if (_nodes_out[u] > 0) {
        auto edge_max = &flat_edge_info[_nodes_offset[u] + InsideArrayRefType{_nodes_out[u]}];
        outflow_u = _nodes_out[u];
        auto edge = &flat_edge_info[_nodes_offset[u]];

        outflow_u--;
        while (edge < edge_max) {
          if (outflow_u > _nodes_out[*edge]) {
            // std::cout << "FLOI" << std::endl;
            flip_active_edge(u, InsideArrayRefType{edge - flat_edge_info.data()});
            outflow_u--;
            edge_max--;
          } else {
            edge++;
          }
        }
      }
      const auto offset = _nodes_offset[u];
      const auto offset_1 = &flat_edge_info[_nodes_offset[u + 1]];
      for (auto edge = &flat_edge_info[offset + InsideArrayRefType{_nodes_out[u]}]; edge < offset_1;
           edge++) {
        if (outflow_u < _nodes_out[*edge] - 1) {
          // std::cout << "FLOI" << std::endl;
          flip_reverse_edge(u, InsideArrayRefType{edge - flat_edge_info.data()});
          outflow_u++;
        }
      }
    }
    //  assert(std::accumulate(_nodes_out.begin(), _nodes_out.end(), 0UL) == flat_edge_info.size() /
    //  2);
  }
  void checkall() {
    {
      IndexType u{0};
      for (auto off : _nodes_offset) {
        for (size_t i = off; i < off + _nodes_out[u]; i++) {
          auto v = flat_edge_info[i];
          auto rel = flat_edge_info_rel_reverse[i];
          assert(_nodes_offset[v] + InsideArrayRefType{rel} < flat_edge_info.size());
          assert(IndexType{i - off} ==
                 flat_edge_info_rel_reverse[_nodes_offset[v] + InsideArrayRefType{rel}]);
          assert(u == flat_edge_info[_nodes_offset[v] + rel]);
        }
        u++;
      }
    }
    {
      for (IndexType u{0}; u < _nodes_out.size() - 1; u++) {
        for (size_t i = _nodes_offset[u]; i < _nodes_offset[u + 1]; i++) {
          auto v = flat_edge_info[i];
          auto rel = flat_edge_info_rel_reverse[i];
          assert(_nodes_offset[v] + InsideArrayRefType{rel} < flat_edge_info.size());
          assert(IndexType{i - _nodes_offset[u]} ==
                 flat_edge_info_rel_reverse[_nodes_offset[v] + InsideArrayRefType{rel}]);
          assert(u == flat_edge_info[_nodes_offset[v] + rel]);
        }
      }
    }
  }
  SmallerGraph(const SmallerGraph&) = delete;
  SmallerGraph(SmallerGraph&&) = delete;
  SmallerGraph& operator=(const SmallerGraph&) = delete;
  SmallerGraph& operator=(SmallerGraph&&) = delete;
  template <class SimpleSetGraph>
  SmallerGraph(const SimpleSetGraph& set_graph)
      : flat_edge_info(2UL * set_graph.edges_),
        flat_edge_info_rel_reverse(2UL * set_graph.edges_),
        _nodes_offset(set_graph.nodes_),
        _nodes_out(set_graph.nodes_, 0),
        instance_(set_graph.nodes_, set_graph.edges_) {
    {
      TIMED_SCOPE(timer, "conversion");

      // setup graph
      IndexType u{0};
      InsideArrayRefType old_of{0};
      std::vector<IndexType> back_sets(set_graph.entries.size());
      for (const auto& out_s : set_graph.entries) {
        _nodes_offset[u] = old_of;
        old_of += out_s.size();
        back_sets[u] = IndexType{out_s.size() - 1};
        u++;
      }
      flat_edge_info.resize(old_of);
      flat_edge_info_rel_reverse.resize(old_of);

      IndexType v = IndexType{0};
      for (const auto& out_s : set_graph.entries) {
        for (auto u : out_s) {
          if (u > v) {
            break;
          }
          flat_edge_info[_nodes_offset[v] + _nodes_out[v]] = IndexType{u};
          flat_edge_info_rel_reverse[_nodes_offset[v] + _nodes_out[v]] = back_sets[u];

          flat_edge_info[_nodes_offset[u] + back_sets[u]] = IndexType{v};
          flat_edge_info_rel_reverse[_nodes_offset[u] + back_sets[u]] = IndexType{_nodes_out[v]};
          back_sets[u]--;
          _nodes_out[v]++;
        }
        v++;
      }
      //   checkall();
    }
  }
  void resetOutdegreeToFull() {
    for (size_t i = 0; i < _nodes_offset.size(); i++) {
      _nodes_out[i] = _nodes_offset[i + 1] - _nodes_offset[i];
    }
    _nodes_out[_nodes_offset.size() - 1] =
        flat_edge_info.size() - _nodes_offset[_nodes_offset.size() - 1];
  }
  void setNodeWeight(IndexType, WeightType) {}
  void setEdgeWeight(IndexType, WeightType) {}
  int edgeSize(IndexType e) { return 2; }

  // bool orientation(size_t edge) { return std::get<DIRECTION>(_edges[edge]); }

  bool valid() { return true; }

  void shrink_to_size() { throw; }
};
template <typename T>
struct is_smaller_type : std::false_type {};
#ifdef AVX_ENABLED_DEFINED
using StandardSmallerGraph = SmallerGraph<int, long>;
#else
using StandardSmallerGraph = SmallerGraph<unsigned int, size_t>;
template <>
struct is_smaller_type<SmallerGraph<int, long> > : std::true_type {};
#endif

template <>
struct is_smaller_type<StandardSmallerGraph> : std::true_type {};

}  // namespace ds
}  // namespace edge_orientation
}  // namespace rpo
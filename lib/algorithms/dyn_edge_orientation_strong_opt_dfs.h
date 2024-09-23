#ifndef DELTA_ORIENTATIONS_strong_opt_dfs_H
#define DELTA_ORIENTATIONS_strong_opt_dfs_H

#include <map>
#include <memory>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "DynEdgeOrientation.h"
#include "dyn_edge_orientation_naive_opt.h"
#include "dyn_graph_access.h"
#include "priority_queues/bucket_pq.h"

#define UPDATE_SECOND(v, offset, update) m_adj[v][offset].second = update;
// if (m_adj[m_adj[v][offset].first].size() < update)                           \
  //   throw;                                                                     \

class dyn_edge_orientation_strong_opt_dfs : public dyn_edge_orientation {
public:
  dyn_edge_orientation_strong_opt_dfs(
      const std::shared_ptr<dyn_graph_access> &GOrientation,
      const DeltaOrientationsConfig &config, DeltaOrientationsResult &result);

  void handleInsertion(NodeID source, NodeID target) override;
  void handleDeletion(NodeID source, NodeID target) override;
  void end() {
    // std::map<int, int> map1;
    for (unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
      // map1[m_out_degree[i]]++;
      for (unsigned j = 0; j < m_out_degree[i]; j++) {
        GOrientation->new_edge(i, m_adj[i][j].first);
      }
    }
    // for (auto [k, v] : map1) {
    //   std::cout << k << " " << v << std::endl;
    // }
  };

  bool adjacent(NodeID source, NodeID target) override {
    for (unsigned i = 0; i < m_out_degree[source]; i++) {
      if (m_adj[source][i].first == target)
        return true;
    }
    for (unsigned i = 0; i < m_out_degree[target]; i++) {
      if (m_adj[target][i].first == source)
        return true;
    }

    return false;
  }

private:
  void flip_active_edge2(NodeID u, NodeID offset) {
    if (m_out_degree[u] <= offset) {
      throw;
    }
    auto [v, in_v_offset] = m_adj[u][offset];
    NodeID last_in_u_active = m_out_degree[u] - 1;
    if (last_in_u_active != offset) {
      auto [in_u_target, ref_active] = m_adj[u][last_in_u_active];
      // update to offset
      m_adj[in_u_target][ref_active].second = offset;

      std::swap(m_adj[u][offset], m_adj[u][last_in_u_active]);
    }
    NodeID first_in_v_passive = m_out_degree[v];
    auto [in_v_target, ref_passive] = m_adj[v][first_in_v_passive];
    if (in_v_offset != first_in_v_passive) {
      m_adj[in_v_target][ref_passive].second = in_v_offset;
      std::swap(m_adj[v][first_in_v_passive], m_adj[v][in_v_offset]);
    }
    m_adj[v][first_in_v_passive].second = last_in_u_active;
    m_adj[u][last_in_u_active].second = first_in_v_passive;
    m_out_degree[v]++;
    m_out_degree[u]--;
  }
  inline void flip_active_edge(NodeID u, NodeID e) {
    if (m_out_degree[u] <= e) {
      throw;
    }
    // checkall();
    // edge oriented u-> v
    auto v = m_adj[u][e].first;
    auto in_v_offset = m_adj[u][e].second;
    // if (in_v_offset < m_out_degree[v]) {
    //   throw;
    // }
    // assert(in_v_offset >= _nodes_out[v]);

    auto &outflow_u = m_out_degree[u];
    // assert(outflow_u > 0);
    auto &outflow_v = m_out_degree[v];

    // move v  in u part to end
    NodeID last_in_u_array_active = outflow_u - 1;
    auto last_in_u_array_target = m_adj[u][last_in_u_array_active].first;
    NodeID last_in_u_array_target_reverse =
        m_adj[u][last_in_u_array_active].second;
    // assert(flat_edge_info[_nodes_offset[last_in_u_array_target] +
    //                       InsideArrayRefType{last_in_u_array_target_reverse}]
    //                       ==
    //        u);
    m_adj[u][e].first = last_in_u_array_target;
    // m_adj[u][e].second = last_in_u_array_target_reverse;
    UPDATE_SECOND(u, e, last_in_u_array_target_reverse)

    if (e != last_in_u_array_active) {
      // m_adj[last_in_u_array_target][last_in_u_array_target_reverse].second =
      // e;
      UPDATE_SECOND(last_in_u_array_target, last_in_u_array_target_reverse, e)
    }

    auto rev_e = in_v_offset;
    // assert(rev_e < flat_edge_info.size());
    // assert(flat_edge_info[rev_e] == u);
    // assert(flat_edge_info_rel_reverse[rev_e] ==
    //        IndexType{e - _nodes_offset[u]});
    // assert(rev_e >= _nodes_offset[v] + _nodes_out[v]);
    // // now write
    // assert(in_v_offset >= _nodes_out[v]);
    NodeID last_in_v_inactive = outflow_v;
    NodeID last_in_v_target = m_adj[v][last_in_v_inactive].first;
    NodeID last_in_v_target_reverse = m_adj[v][last_in_v_inactive].second;

    m_adj[v][rev_e].first = last_in_v_target;
    UPDATE_SECOND(v, rev_e, last_in_v_target_reverse)
    if (rev_e != last_in_v_inactive) {
      UPDATE_SECOND(last_in_v_target, last_in_v_target_reverse, in_v_offset)
    }
    // now write u
    m_adj[v][last_in_v_inactive].first = u;

    // m_adj[v][last_in_v_inactive].second = outflow_u - 1;
    UPDATE_SECOND(v, last_in_v_inactive, outflow_u - 1)

    // now write v into the last position
    m_adj[u][last_in_u_array_active].first = v;
    // m_adj[u][last_in_u_array_active].second = outflow_v;
    UPDATE_SECOND(u, last_in_u_array_active, outflow_v)

    // assert(last_in_u_array_target == flat_edge_info[e]);
    outflow_v++;
    outflow_u--;
    // checkall();
  }
  void flip_reverse_edge(NodeID u, NodeID offset) {
    flip_active_edge(m_adj[u][offset].first, m_adj[u][offset].second);
  }
  void dfs_forward(NodeID source);
  void dfs_backward(NodeID target);
  void dfs_forward_correct_2(NodeID source);
  void dfs_backward_correct_2(NodeID target);

  // int checkmaxdegree(char error) {
  //   return 0;
  //   int ma = 0;
  //   for (const auto d : m_out_degree) {
  //     ma = std::max(d, ma);
  //   }
  //   int u = 0;
  //   for (auto &neib : m_adj) {
  //     for (auto [v, x] : neib) {
  //       if (m_adj[v][x].first != u) {
  //         std::cout << "FLAWED" << error << std::endl;
  //         throw;
  //       }
  //     }
  //     u++;
  //   }
  //   return ma;
  // }
  void checkdegrees() {
    int d = 0;
    for (auto m : m_adj) {
      std::cout << m_out_degree[d++] << " ";
      for (auto [p, v] : m) {
        std::cout << p << "(" << v << ") ";
        if (m_adj[p].size() <= v) {
          std::cout << "PROBLEM" << std::endl;
        }
      }
      std::cout << std::endl;
    }
    /*int max = 0;
    for (unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
      if (m_degree[i] != m_adj[i].size()) {
        std::cout << "problem with degrees" << std::endl;
        exit(0);
      }
      if (m_adj[i].size() > max) {
        max = m_adj[i].size();
      }
    }
    for (unsigned i = 0; i < GOrientation->number_of_nodes(); i++) {
      if (m_degree_buckets[m_degree[i]].size() < m_bucket_pos[i]) {
        std::cout << "size problem " << m_bucket_pos[i] << " "
                  << m_degree_buckets[m_degree[i]].size() << std::endl;
        exit(0);
      }
      if (m_degree_buckets[m_degree[i]][m_bucket_pos[i]] != i) {
        std::cout << "bucket pos error" << std::endl;
        exit(0);
      }
    }
    if (max != m_max_degree) {
      std::cout << "max degree does not match" << std::endl;
      exit(0);
    }*/
  };

  std::vector<std::pair<int, int>> m_parent;
  std::vector<bool> visited;
  std::vector<int> m_out_degree; // out degree
  // First is the neighbor id, second is the offset in its m_adj subvector
  std::vector<std::vector<std::pair<NodeID, NodeID>>> m_adj;

  std::vector<unsigned> child_index;
};

#endif
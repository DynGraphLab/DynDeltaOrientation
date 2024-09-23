//
//

#include "dyn_edge_orientation_improved_opt.h"
#include "random_functions.h"
#include <cmath>
#include <tuple>

dyn_edge_orientation_improved_opt::dyn_edge_orientation_improved_opt(
    const std::shared_ptr<dyn_graph_access> &GOrientation,
    const DeltaOrientationsConfig &config, DeltaOrientationsResult &result)
    : dyn_edge_orientation(GOrientation, config, result) {
  m_out_degree.resize(GOrientation->number_of_nodes(), 0);
  m_parent.resize(GOrientation->number_of_nodes(), {0, 0});
  visited.resize(GOrientation->number_of_nodes(), 0);
  m_adj.resize(GOrientation->number_of_nodes());
}

void dyn_edge_orientation_improved_opt::handleInsertion(NodeID source, NodeID target) {
  if (m_out_degree[source] > m_out_degree[target]) {
    std::swap(target, source);
  }
  // std::cout << m_out_degree[source] << " " << m_out_degree[target] <<
  // std::endl;
  //  W.l.o.g: ODEG(source) <= ODEG(target)
  //  first insert in target (easy)
  m_adj[target].push_back({source, m_out_degree[source]});
  // inserting in source array
  m_adj[source].push_back({target, m_adj[target].size() - 1});
  if (m_out_degree[source] != m_adj[source].size() - 1) {
    std::swap(m_adj[source].back(), m_adj[source][m_out_degree[source]]);
    auto [t, ref_t] = m_adj[source].back();
    // m_adj[t][ref_t].second = m_adj[source].size() - 1;
    UPDATE_SECOND(t, ref_t, m_adj[source].size() - 1)
  }
  m_out_degree[source]++;

  // checkdegrees();
  //  std::cout << "BFS" << std::endl;
  //bfs_forward(source);
  //  std::cout << "BFS_after" << std::endl;
  //   if (o > 7)
  //     std::cout << "HANDLE INSERT" << o << std::endl;
  // checkdegrees();
  //  TODO update
  // if (cheat.outdegree() != o) {
  //   size_t s = 0;
  //   for (auto &e : m_adj) {
  //     s += e.size();
  //   }
  //   std::cout << s << std::endl;
  //   std::cout << "ALERT Insert" << std::endl;
  //   throw;
  // }

  //Regain optimality
  if(m_out_degree[source] == max_out_degree){
    if(!bfs_forward(source)){
      max_node_count++;
    }
  }else if(m_out_degree[source] == max_out_degree + 1){
    if(bfs_forward(source)){
      max_node_count++;
    }else{
      max_out_degree++;
      max_node_count = 1;
    }
  }
}

void dyn_edge_orientation_improved_opt::handleDeletion(NodeID source, NodeID target) {
  //   if (o > 7)
  // std::cout << "HANDLE DELETE" << std::endl;
  NodeID decremented;
  NodeID ref_target = 0;
  bool gound = false;
  // First find the target in source's list and update
  for (unsigned i = 0; i < m_adj[source].size(); i++) {
    if (m_adj[source][i].first == target) {
      gound = true;
      // This ref_target is used needed to find the other entry later.
      ref_target = m_adj[source][i].second;
      // We now swap out the value
      if (i < m_out_degree[source]) { // by first swapping with the
                                      // out_degree-1 one
        std::swap(m_adj[source][i], m_adj[source][m_out_degree[source] - 1]);
        auto [new_p, ref] = m_adj[source][i];
        UPDATE_SECOND(new_p, ref, i)

        // for outer loop set
        i = m_out_degree[source] - 1;
        m_out_degree[source]--;
        decremented = source;
      }
      // last swap
      std::swap(m_adj[source][i], m_adj[source][m_adj[source].size() - 1]);
      if (i != m_adj[source].size() - 1) { // update only needed when unequal
        auto [new_p, ref] = m_adj[source][i];
        UPDATE_SECOND(new_p, ref, i)
      }
      m_adj[source].pop_back();
      break;
    }
  }
  assert(m_adj[target][ref_target].first == source);
  // Secondly use the pointer to find other entry.
  if (ref_target < m_out_degree[target]) {
    if (m_out_degree[target] - 1 != ref_target) {
      std::swap(m_adj[target][ref_target],
                m_adj[target][m_out_degree[target] - 1]);
      auto [new_p, ref] = m_adj[target][ref_target];
      // m_adj[new_p][ref].second = ref_target;
      UPDATE_SECOND(new_p, ref, ref_target)

      ref_target = m_out_degree[target] - 1;
    }
    m_out_degree[target]--;
    decremented = target;
  }
  // last swap
  std::swap(m_adj[target][ref_target], m_adj[target][m_adj[target].size() - 1]);
  if (ref_target !=
      m_adj[target].size() - 1) { // update only needed when unequal
    auto [new_p, ref] = m_adj[target][ref_target];
    //  m_adj[new_p][ref].second = ref_target;
    UPDATE_SECOND(new_p, ref, ref_target)
  }
  m_adj[target].pop_back();
  // int o = checkmaxdegree('d');
  // TODO: Update Routine.
  //bfs_backward(decremented);

  //   if (o > 7)
  //     std::cout << "HANDLE INSERT" << o << std::endl;
  // checkdegrees();
  //  TODO update

  //Regain optimality
  if(m_out_degree[decremented] == max_out_degree - 1){
    max_node_count--;
  }else if(m_out_degree[decremented] == max_out_degree - 2){
    if(bfs_backward(decremented)){
      max_node_count--;
    }
  }
  if(max_node_count == 0){
    max_out_degree--;
    find_optimal();
  }
}

bool dyn_edge_orientation_improved_opt::bfs_forward(NodeID source) {
  if(m_out_degree[source] > 1){
    std::vector<NodeID> touched;
    std::queue<NodeID> bfsqueue;
    int m_max_degree = m_out_degree[source] - 1;

    bfsqueue.push(source);
    m_parent[source] = {source, 0};
    visited[source] = 1;
    touched.push_back(source);

    bool found_lower_deg_vertex = false;
    NodeID final_vertex;

    while (!bfsqueue.empty()) {
      NodeID node = bfsqueue.front();
      bfsqueue.pop();

      unsigned out_d = m_out_degree[node];
      for (unsigned i = 0; i < out_d; i++) {
        NodeID target = m_adj[node][i].first;
        if (visited[target] == 0 && m_out_degree[target] <= m_max_degree) {
          visited[target] = 1;
          bfsqueue.push(target);
          m_parent[target] = {node, i};
          touched.push_back(target);
          if (m_out_degree[target] < m_max_degree) {
            found_lower_deg_vertex = true;
            final_vertex = target;
            break;
          }
        }
      }
      if (found_lower_deg_vertex) {
        break;
      }
    }

    if (found_lower_deg_vertex) {
      // follow path and swap it
      NodeID curNode = final_vertex;
      std::vector<std::tuple<NodeID, NodeID, NodeID>> res;
      while (m_parent[curNode].first != curNode) {
        NodeID parent = m_parent[curNode].first;
        res.push_back({parent, m_parent[curNode].second, curNode});
        curNode = m_parent[curNode].first;
      };
      for (auto i = res.rbegin(); i != res.rend(); i++) {
        // std::cout << i - res.rbegin() << std::endl;
        // std::cout << std::get<0>(*i) << " "
        //           << m_adj[std::get<0>(*i)][std::get<1>(*i)].first << " "
        //           << std::get<2>(*i) << std::endl;
        flip_active_edge(std::get<0>(*i), std::get<1>(*i));
      }
    }

    for (NodeID v : touched) {
      //m_parent[v] = {0, 0};
      visited[v] = 0;
    }

    return found_lower_deg_vertex;
  }else{
    return false;
  }
}

bool dyn_edge_orientation_improved_opt::bfs_backward(NodeID source) {
  std::vector<NodeID> touched;
  std::queue<NodeID> bfsqueue;
  int m_max_degree = m_out_degree[source] + 1;

  bfsqueue.push(source);
  m_parent[source] = {source, 0};
  visited[source] = 1;
  touched.push_back(source);

  bool found_lower_deg_vertex = false;
  NodeID final_vertex;

  while (!bfsqueue.empty()) {
    NodeID node = bfsqueue.front();
    bfsqueue.pop();

    unsigned out_d = m_out_degree[node];
    for (unsigned i = out_d; i < m_adj[node].size(); i++) {
      NodeID target = m_adj[node][i].first;
      if (visited[target] == 0 && m_out_degree[target] >= m_max_degree) {
        visited[target] = 1;
        bfsqueue.push(target);
        m_parent[target] = {node, i};
        touched.push_back(target);
        if (m_out_degree[target] > m_max_degree) {
          found_lower_deg_vertex = true;
          final_vertex = target;
          break;
        }
      }
    }
    if (found_lower_deg_vertex) {
      break;
    }
  }

  if (found_lower_deg_vertex) {
    // follow path and swap it
    NodeID curNode = final_vertex;
    std::vector<std::pair<NodeID, NodeID>> path;
    while (m_parent[curNode].first != curNode) {
      NodeID parent = m_parent[curNode].first;
      path.push_back({parent, m_parent[curNode].second});
      curNode = m_parent[curNode].first;
    };
    for (auto i = path.begin(); i != path.end(); i++) {
      flip_reverse_edge(i->first, i->second);
    }
  }

  for (NodeID v : touched) {
    //m_parent[v] = {0, 0};
    visited[v] = 0;
  }

  return found_lower_deg_vertex;
}

void dyn_edge_orientation_improved_opt::find_optimal() {
  if(max_out_degree > 1){
    //std::cout << "Start find optimal: " << std::endl;
    bool path_improved = true;
    while(path_improved){
      path_improved = false;
      bool all_path_improved = true;
      for(unsigned i = 0; i < m_adj.size(); i++){
        if(m_out_degree[i] == max_out_degree){
          if(bfs_forward(i)){
            path_improved = true;
          }else{
            all_path_improved = false;
          }
        }
      }
      if(all_path_improved){
        max_out_degree--;
        //std::cout << "Lets go deeper" << std::endl;
      }
      if(path_improved && !all_path_improved){
        //std::cout << "Another Round" << std::endl;
      }
    }
    //Assert: max_node_count == 0
    for(unsigned i = 0; i < m_adj.size(); i++){
      if(m_out_degree[i] == max_out_degree){
        max_node_count++;
      }
    }
  }
}
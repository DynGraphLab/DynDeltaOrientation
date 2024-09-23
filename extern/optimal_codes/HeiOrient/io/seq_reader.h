#pragma once
#include <iostream>
#include <istream>
#include <memory>
#include <sstream>

#include "absl/status/statusor.h"
#include "absl/strings/ascii.h"
#include "absl/strings/string_view.h"
#include "utils/logging.h"

namespace rpo {
namespace io {
template <class Graph>
absl::StatusOr<std::unique_ptr<Graph>> readSeq(std::istream& input, bool verbose = false) {
  TIMED_FUNC(timerObj);

  if (!input) {
    return absl::NotFoundError("stream not good error");
  }
  std::string line;
  int num_nodes = 0;
  int num_edges = 0;
  // int undirected = 0;

  std::getline(input, line);
  std::stringstream first_line(line);

  std::string hash;
  first_line >> hash;
  if (hash != "#") {
    return absl::InvalidArgumentError("META DATA SEEMS TO BE MISSING");
  }
  if (!(first_line >> num_nodes)) {
    return absl::InvalidArgumentError("NUM_NODES SEEMS TO BE MISSING");
  }
  if (!(first_line >> num_edges)) {
    return absl::InvalidArgumentError("NUM_EDGES SEEMS TO BE MISSING");
  }
  auto graph = std::make_unique<Graph>(num_edges, num_nodes);
  while (std::getline(input, line)) {
    typename Graph::NodeType u = 0;
    typename Graph::NodeType v = 0;
    int ins_del = 0;

    std::stringstream ss(line);
    ss >> ins_del;
    ss >> u;
    ss >> v;

    if (ins_del == 1) {
      graph->addEdge(u, v, 1);
      graph->addEdge(v, u, 1);
    } else {
      return absl::UnimplementedError("Other modes are not implemented");
    }
    num_edges--;
  }
  graph->shrink_to_size();
  return graph;
}
template <class Graph>
absl::StatusOr<std::unique_ptr<Graph>> readSeqWithDeletions(std::istream& input,
                                                            bool verbose = false) {
  TIMED_FUNC(timerObj);

  if (!input) {
    return absl::NotFoundError("stream not good error");
  }
  std::string line;
  int num_nodes = 0;
  int num_edges = 0;
  // int undirected = 0;

  std::getline(input, line);
  std::stringstream first_line(line);

  std::string hash;
  first_line >> hash;
  if (hash != "#") {
    return absl::InvalidArgumentError("META DATA SEEMS TO BE MISSING");
  }
  if (!(first_line >> num_nodes)) {
    return absl::InvalidArgumentError("NUM_NODES SEEMS TO BE MISSING");
  }
  if (!(first_line >> num_edges)) {
    return absl::InvalidArgumentError("NUM_EDGES SEEMS TO BE MISSING");
  }
  auto graph = std::make_unique<Graph>(num_edges, num_nodes);
  while (std::getline(input, line)) {
    typename Graph::NodeType u = 0;
    typename Graph::NodeType v = 0;
    int ins_del = 0;

    std::stringstream ss(line);
    ss >> ins_del;
    ss >> u;
    ss >> v;

    if (ins_del == 1) {
      graph->addEdge(u, v, 1);
      graph->addEdge(v, u, 1);
    } else {
      graph->removeEdge(u, v);
      graph->removeEdge(v, u);
    }
    num_edges--;
  }
  graph->shrink_to_size();
  return graph;
}
}  // namespace io
}  // namespace rpo

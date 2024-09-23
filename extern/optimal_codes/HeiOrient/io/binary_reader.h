#pragma once
#include <iostream>
#include <istream>

#include "absl/status/statusor.h"
#include "io/binary_utils.h"
#include "utils/logging.h"

namespace rpo {
namespace io {
template <typename T>
T readBin(std::istream& istream) {
  T t;
  istream.read(reinterpret_cast<char*>(&t), sizeof(T));
  return t;
}
template <class Graph>
absl::StatusOr<std::unique_ptr<Graph>> readBinaryUndirectedGraph(std::istream& istream) {
  return absl::UnimplementedError("not implemented");
}
template <class Graph>
absl::StatusOr<std::unique_ptr<Graph>> readBinaryUndirectedUnweightedGraph(std::istream& istream) {
  TIMED_FUNC(fessf);
  size_t version = ntohll(readBin<size_t>(istream));

  if (version == 1) {
    uint64_t size_info = ntohll(readBin<size_t>(istream));
    if (size_info != sizeof(size_t)) {
      return absl::UnimplementedError("size mismatch.");
    }
    size_t vertex_c = ntohll(readBin<size_t>(istream));
    auto graph = std::make_unique<Graph>(0, vertex_c);
    for (size_t u = 0; u < vertex_c; u++) {
      size_t number_of_edges = ntohll(readBin<size_t>(istream));
      for (size_t i = 0; i < number_of_edges; i++) {
        typename Graph::NodeType v = ntohll(readBin<size_t>(istream));
        graph->addEdge(u, v, 1);
        graph->addEdge(v, u, 1);
      }
    }
    graph->finish();
    return graph;
  }
  if (version != 2) {
    return absl::UnimplementedError("version header did not match");
  }
  uint64_t size_info = ntohll(readBin<size_t>(istream));
  if (size_info != sizeof(size_t)) {
    return absl::UnimplementedError("size mismatch.");
  }
  size_t vertex_c = ntohll(readBin<size_t>(istream));
  auto graph = std::make_unique<Graph>(0, vertex_c);
  for (size_t u = 0; u < vertex_c; u++) {
    size_t number_of_edges = ntohl(readBin<uint32_t>(istream));
    for (size_t i = 0; i < number_of_edges; i++) {
      typename Graph::NodeType v = ntohl(readBin<uint32_t>(istream));
      graph->addEdge(u, v, 1);
      graph->addEdge(v, u, 1);
    }
  }
  graph->finish();
  return graph;
}
}  // namespace io
}  // namespace rpo
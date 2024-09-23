#pragma once

#include <iostream>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/match.h"
#include "utils/logging.h"

namespace rpo {
namespace io {
/**
 * @brief Reads Graph from istream and stores them in Hypergraph.
 *
 * @tparam Hypergraph
 * @param stream
 * @return Hypergraph
 */
template <class Hypergraph>
Hypergraph readMtxAsUndirectedGraph(std::istream& stream) {
  // read header
  std::string header;
  bool header_read = false;
  bool symmetric = false;
  while (header == "") {
    if (stream.eof()) {
      std::cerr << "File does not contain enough lines. Could not read header." << std::endl;
      exit(1);  // TODO(author): should we throw an exception?
    }
    std::getline(stream, header);
    header_read |= absl::StrContains(header, "MatrixMarket");
    if (absl::StrContains(header, "MatrixMarket") && absl::StrContains(header, "coordinate")) {
      symmetric = absl::StrContains(header, "symmetric");
    }
    if (header.length() > 0 && header[0] == '%') {
      header = "";
    }
  }
  if (!symmetric) {
    std::cerr << " Only symmetric are supported" << std::endl;
    exit(1);
  }
  std::stringstream s(header);
  int num_rows, num_cols, num_nonzeros;
  s >> num_cols;
  s >> num_rows;
  if (num_cols != num_rows) {
    std::cerr << "Num cols != num_rows" << std::endl;
    exit(1);
  }
  s >> num_nonzeros;

  int read_in = 0;
  int same_coord = 0;
  std::vector<double> node_weights(num_cols, 1);
  std::vector<double> edge_weights;
  std::vector<std::pair<int, int>> edges;

  while (read_in < num_nonzeros && !stream.eof()) {
    std::string line;
    std::getline(stream, line);
    std::stringstream line_stream(line);
    // std::cerr << line << std::endl;
    if (line != "") {
      int x, y;
      double weight;
      if (line_stream >> x) {
        if (line_stream >> y) {
          if (line_stream >> weight) {
            if (x == y) {
              same_coord++;
              node_weights[x - 1] = weight;
              continue;
            }
            edges.push_back(std::make_pair<>(x - 1, y - 1));
            edge_weights.push_back(weight);
            read_in++;
          } else {
            std::cerr << "Weight failed" << read_in << std::endl;
            exit(1);
          }
        } else {
          std::cerr << "y failed" << read_in << std::endl;
          exit(1);
        }
      } else {
        std::cerr << line << std::endl;
        std::cerr << read_in << std::endl;
        break;
      }
    }
  }
  Hypergraph hypergraph(edge_weights.size(), num_cols, false, false);
  int n = 0;
  for (auto [x, y] : edges) {
    hypergraph.establishConnection(n, y);
    hypergraph.establishConnection(n, x);
    hypergraph.setEdgeWeight(n, edge_weights[n]);
    n++;
  }

  std::cerr << "Same coord " << same_coord << std::endl;
  for (int n = 0; n < hypergraph.initialNumNodes(); n++) {
    if (hypergraph.nodeWeight(n) == 0) {
      hypergraph.setNodeWeight(n, 1);
    }
  }
  for (auto e : hypergraph.edges()) {
    if (hypergraph.edgeSize(e) != 2) {
      std::cerr << e << " has  size  " << hypergraph.edgeSize(e) << std::endl;
    }
  }
  for (int e = 0; e < hypergraph.initialNumEdges(); e++) {
    if (hypergraph.edgeSize(e) != 2) {
      std::cerr << e << " has  size  " << hypergraph.edgeSize(e) << std::endl;
    }
  }
  int zeros = 0;
  for (int n = 0; n < hypergraph.initialNumNodes(); n++) {
    if (hypergraph.nodeDegree(n) == 0) {
      zeros++;
    }
  }
  if (zeros != 0) {
    std::cerr << "[WARN] there are " << zeros << " zero nodes" << std::endl;
  }
  hypergraph.sort();
  return hypergraph;
}
// Reads mtx as graph
template <class Graph>
absl::StatusOr<std::unique_ptr<Graph>> readMtxGraph(std::istream& stream) {
  TIMED_FUNC(timer);
  // read header
  std::string header;
  bool header_read = false;
  bool symmetric = false;
  while (header == "") {
    if (stream.eof()) {
      return absl::NotFoundError("File does not contain enough lines. Could not read header.");
    }
    std::getline(stream, header);
    header_read |= absl::StrContains(header, "MatrixMarket");
    if (absl::StrContains(header, "MatrixMarket") && absl::StrContains(header, "coordinate")) {
      symmetric = absl::StrContains(header, "symmetric");
    }
    if (header.length() > 0 && header[0] == '%') {
      header = "";
    }
  }
  if (!symmetric) {
    return absl::UnimplementedError("Only symmetric are supported");
  }
  std::stringstream s(header);
  typename Graph::NodeType num_rows, num_cols, num_nonzeros;
  s >> num_cols;
  s >> num_rows;
  if (num_cols != num_rows) {
    return absl::OutOfRangeError("Num cols != num_rows");
    exit(1);
  }
  s >> num_nonzeros;

  typename Graph::NodeType read_in = 0;
  typename Graph::NodeType same_coord = 0;
  std::vector<std::pair<typename Graph::NodeType, typename Graph::NodeType>> edges;

  while (read_in < num_nonzeros && !stream.eof()) {
    std::string line;
    std::getline(stream, line);
    std::stringstream line_stream(line);
    // std::cerr << line << std::endl;
    if (line != "") {
      typename Graph::NodeType x, y;
      double weight;
      line_stream >> x;
      line_stream >> y;
      line_stream >> weight;

      if (x == y) {
        same_coord++;
        continue;
      }
      edges.push_back(std::make_pair<>(x - 1, y - 1));

      read_in++;
    }
  }

  auto graph = std::make_unique<Graph>(edges.size(), num_cols);
  typename Graph::EdgeType n = 0;
  for (auto [x, y] : edges) {
    graph->addEdge(x, y, 1);
    graph->addEdge(y, x, 1);
  }
  graph->finish();
  return graph;
}
}  // namespace io
}  // namespace rpo
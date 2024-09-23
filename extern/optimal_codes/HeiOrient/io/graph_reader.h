#pragma once
#include <iomanip>
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
template <class Hypergraph>
absl::StatusOr<std::unique_ptr<Hypergraph>> readGraph(std::istream& stream, bool verbose = false,
                                                      bool readAll = false) {
  TIMED_FUNC(timerObj);

  if (!stream.good()) {
    return absl::NotFoundError("stream not good error");
  }
  std::string header;
  while (header == "") {
    if (stream.eof()) {
      return absl::NotFoundError("File does not contain enough lines. Could not read header.");
    }
    std::getline(stream, header);
    if (header.length() > 0 && header[0] == '%') {
      header = "";
    }
  }
  std::stringstream s(header);
  size_t num_edges = 0;
  size_t num_nodes = 0;
  size_t mode = 0;
  if (!(s >> num_nodes)) {
    return absl::NotFoundError("Failure to read in num_nodes");
  }
  if (!(s >> num_edges)) {
    return absl::NotFoundError("Failure to read in num_edges");
  }
  if (!(s >> mode)) {
    mode = 0;
  }
  if (true) {
    std::cout << num_edges << " " << num_nodes << std::endl;
  }
  if (std::numeric_limits<typename Hypergraph::NodeType>::max() < num_nodes) {
    return absl::UnimplementedError("num_nodes exceeds variables");
  }
  if (((size_t)std::numeric_limits<typename Hypergraph::NodeType>::max()) < num_edges) {
    return absl::UnimplementedError("num_edges exceeds variables");
  }
  auto h = std::make_unique<Hypergraph>(num_edges, num_nodes);

  const bool read_in_node_weight = (mode / 10) == 1;
  for (typename Hypergraph::NodeType node = 0; node < ((typename Hypergraph::NodeType)num_nodes);
       node++) {
    std::string str = "%";
    if (stream.eof()) {
      std::stringstream s;
      s << "File does not contain enough lines. Read " << node << ". Expected " << num_nodes
        << std::endl;
      return absl::NotFoundError(s.str());
    }
    while ((str != "" && str[0] == '%')) {
      std::getline(stream, str);
      absl::StripLeadingAsciiWhitespace(&str);
    };
    if (!stream) {
      std::stringstream s;
      s << "File does not contain enough lines. Read " << node << ". Expected " << num_nodes
        << std::endl;
      return absl::NotFoundError(s.str());
    }
    std::stringstream line_stream{str};
    typename Hypergraph::WeightType weight = 1;
    // Check if we read in weights
    if (read_in_node_weight) {
      line_stream >> weight;
    }
    h->setNodeWeight(node, weight);
    const bool read_in_edge_weight = mode % 10 == 1;
    typename Hypergraph::NodeType node2;
    std::vector<std::pair<typename Hypergraph::NodeType, typename Hypergraph::WeightType>> elements;
    while (line_stream >> node2) {
      typename Hypergraph::WeightType edge_weight = 1;
      if (read_in_edge_weight) {
        line_stream >> edge_weight;
      }
      if (readAll || node < node2) {
        elements.push_back(std::make_pair(node2, edge_weight));
      }
    }
    std::sort(elements.begin(), elements.end(), [](auto a, auto b) { return a.first < b.first; });
    for (auto [node2, weight] : elements) {
      h->addEdge(node, node2 - 1, weight);
    }
    if (verbose && node % 100000 == 0) {
      std::cout << node << " loaded. " << std::setprecision(2) << 100.0 * node / (double)num_nodes
                << "%" << std::endl;
    }
  }
  if (verbose) {
    std::cout << "FINISHED LOADING" << std::endl;
  }
  h->finish();
  return h;
}
}  // namespace io
}  // namespace rpo
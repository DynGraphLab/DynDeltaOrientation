#include <fstream>
#include <set>

#include "google/protobuf/text_format.h"
#include "io/hgr_reader.h"
#include "io/hgr_writer.h"
#include "runner/hypergraph_storage_system.h"

namespace {
using rpo::ds::StandardIntegerHypergraph;
using rpo::runner::hypergraph_storage_system::HypergraphStorageSystem;
}  // namespace
int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Please provide only one path." << std::endl;
    return 1;
  }
  std::string path(argv[1]);
  int updated_count = 0;
  HypergraphStorageSystem hs(path);
  auto collections = hs.path_collections();
  std::map<std::string, int> node_weight_types;
  std::map<std::string, int> edge_weight_types;
  for (auto& [col_path, col] : collections) {
    std::cout << "Checking (skip/yes)?" << col.collection_name() << std::endl;
    std::string control;
    std::cin >> control;
    if (control == "skip") {
      continue;
    }
    std::cout << "Checking collection" << std::endl;
    std::vector<rpo::app::app_io::Hypergraph> graphs;
    for (auto hypergraph : col.hypergraphs()) {
      if (hypergraph.format() != "hgr") {
        continue;
      }
      if (hypergraph.node_weight_type() == "initial/1") {
        *hypergraph.mutable_node_weight_type() = "random(degree)";
      } else {
        node_weight_types[hypergraph.node_weight_type()] += 1;
      }
      if ((hypergraph.edge_weight_type() == "random(100)" ||
           hypergraph.edge_weight_type() == "random100") &&
          (hypergraph.node_weight_type() == "random(degree)")) {
        for (auto node_w : {"unweighted", "random(degree)"}) {
          *hypergraph.mutable_node_weight_type() = node_w;
          *hypergraph.mutable_edge_weight_type() = "random(100)";
          graphs.push_back(hypergraph);
          *hypergraph.mutable_edge_weight_type() = "pins";
          graphs.push_back(hypergraph);
          *hypergraph.mutable_edge_weight_type() = "uniform";
          graphs.push_back(hypergraph);
        }
      } else {
        edge_weight_types[hypergraph.edge_weight_type()] += 1;
      }
    }
    *col.mutable_hypergraphs() = {graphs.begin(), graphs.end()};
    std::ofstream exp_file(col_path + "/collection.textproto");
    std::string result;
    google::protobuf::TextFormat::PrintToString(col, &result);
    exp_file << result;
  }
  for (auto [k, v] : edge_weight_types) {
    std::cout << k << " " << v << std::endl;
  }
  std::cout << "nodes:" << std::endl;
  for (auto [k, v] : node_weight_types) {
    std::cout << k << " " << v << std::endl;
  }
  std::cout << "Found " << updated_count << " hypergraphs to update" << std::endl;
}
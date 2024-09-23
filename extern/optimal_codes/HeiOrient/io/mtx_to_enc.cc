#include <fstream>

#include "io/binary_reader.h"
#include "io/binary_writer.h"
#include "io/mtx_reader.h"

class DoubleCalledGraphWrapper {
  std::vector<std::vector<size_t>> graph;

 public:
  using NodeType = int64_t;
  using EdgeType = NodeType;
  DoubleCalledGraphWrapper(EdgeType e, size_t n) {
    graph.resize(n);
    if (n > ((size_t)std::numeric_limits<uint32_t>::max())) {
      std::cout << "WARNING oversize" << std::endl;
    }
  }
  void addEdge(NodeType u, NodeType v, double w) {
    if (u < v) {
      graph[u].push_back(v);
    }
  }
  void finish() {}
  void save(std::string file_name) {
    using rpo::io::writeBin;
    std::ofstream output(file_name);
    writeBin(htonll(size_t{2}), output);
    writeBin(htonll(sizeof(size_t)), output);
    writeBin(htonll(graph.size()), output);
    for (auto& edges : graph) {
      writeBin(htonl((uint32_t)edges.size()), output);
      for (size_t v : edges) {
        writeBin(htonl((uint32_t)v), output);
      }
    }
  }
};
int main(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    std::cout << argv[i] << std::endl;
    std::ifstream datei(argv[i]);
    auto res = rpo::io::readMtxGraph<DoubleCalledGraphWrapper>(datei);
    if (!res.ok()) {
      std::cout << argv[i] << " failed " << res.status() << std::endl;
      continue;
    }
    auto graph = std::move(res.value());
    graph->save(std::string(argv[i]) + ".32");
  }
}
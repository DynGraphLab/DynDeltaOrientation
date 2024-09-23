#include <fstream>

#include "ds/modifiable_hypergraph.h"
#include "io/hgr_writer.h"
#include "io/mtx_reader.h"

namespace {
using rpo::ds::StandardIntegerHypergraph;
}
int main(int argc, char** argv) {
  if (argc != 3) {
    return 1;
  }
  std::ifstream istream(argv[1]);
  std::ofstream ostream(argv[2]);
  std::ofstream ostream2(std::string(argv[2]) + ".mtx");

  StandardIntegerHypergraph hypergraph =
      rpo::io::readMtxAsUndirectedGraph<StandardIntegerHypergraph>(istream);
  for (auto e : hypergraph.edges()) {
    hypergraph.setEdgeWeight(e, rand() % 100 + 1);
  }
  rpo::io::writeHypergraph(hypergraph, ostream);
  rpo::io::writeHypergraphMTXUnigraph(hypergraph, ostream2);
}
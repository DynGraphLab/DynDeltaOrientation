#include <fstream>
#include <vector>

#include "ds/empty_problem.h"
#include "edge_orientation/ds/smaller_graph.h"
#include "edge_orientation/tools/combined.h"
#include "edge_orientation/tools/two_approx.h"
#include "io/seq_reader.h"

namespace {
using rpo::edge_orientation::ds::StandardSmallerGraph;
}
StandardSmallerGraph conditional_two_approximation(rpo::ds::StandardSimpleSetGraph& graph,
                                                   double density_to_run_twoapprox) {
  if (((double)graph.edges_) / ((double)graph.nodes_) > density_to_run_twoapprox) {
    auto two_approx = rpo::edge_orientation::two_approx::compute_two_approx(graph);
    std::cout << "Found 2-Approx: " << two_approx << std::endl;
    auto res = rpo::edge_orientation::two_approx::remove_up_to(graph, std::floor(two_approx / 2.0));
    return StandardSmallerGraph(*res);
  }
  return StandardSmallerGraph(graph);
}

int main(int argc, char** argv) {
  std::cout << "Loading " << argv[1] << "." << std::endl;
  std::ifstream datei(argv[1]);
  auto hg_s = rpo::io::readSeqWithDeletions<rpo::ds::StandardSimpleSetGraph>(datei, false);
  if (!hg_s.ok()) {
    std::cerr << "Failed loading" << std::endl;
    return 1;
  }
  auto& graph = *hg_s.value().get();

  auto start = std::chrono::high_resolution_clock::now();
  // conditional two approximation
  std::cout << "Loaded " << argv[1] << "." << std::endl;
  auto orientation = conditional_two_approximation(graph, 10.0);
  orientation.resort_fast();
  int64_t count = sqrt((orientation.computeOutflows()) -
                       (orientation.edge_count() / (double)orientation.vertex_count()));
  int64_t eager_size = 100;
  rpo::edge_orientation::combined::solve_by_dfs_combined_multiple(orientation, count, true,
                                                                  eager_size);

  std::cout << "TOOK: "
            << std::chrono::duration_cast<std::chrono::duration<double>>(
                   std::chrono::high_resolution_clock::now() - start)
                   .count()
            << std::endl;

  std::cout << argv[1] << " has min max out degree " << orientation.computeOutflows() << std::endl;
  return 0;
}

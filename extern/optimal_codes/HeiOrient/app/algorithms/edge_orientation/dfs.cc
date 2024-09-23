#include "app/algorithms/edge_orientation/dfs.h"

#include "edge_orientation/ds/smaller_graph.h"

namespace rpo::app::algorithms::edge_orientation {
namespace {
using SmallerDFS = rpo::app::algorithms::edge_orientation::DfsBased<
    rpo::edge_orientation::ds::StandardSmallerGraph>;
using SmallerDFSNaive = DfsNaive<rpo::edge_orientation::ds::StandardSmallerGraph>;
}  // namespace
REGISTER_IMPL(SFlipGraphDFSImpl);
REGISTER_IMPL(SepSFlipGraphDFSImpl);
REGISTER_IMPL(SmallerDFS);

REGISTER_IMPL(SmallerDFSNaive);

}  // namespace rpo::app::algorithms::edge_orientation
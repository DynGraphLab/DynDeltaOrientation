#include "app/algorithms/edge_orientation/bfs.h"

#include "edge_orientation/ds/flip_graph_seperated.h"

namespace rpo::app::algorithms::edge_orientation {
namespace {
using BfsBased =
    rpo::app::algorithms::BfsBased<rpo::edge_orientation::ds::StandardFlipGraph>;
using BfsBasedSep =
    rpo::app::algorithms::BfsBased<rpo::edge_orientation::ds::StandardSepFlipGraph>;
using BfsBasedSmall =
    rpo::app::algorithms::BfsBased<rpo::edge_orientation::ds::StandardSmallerGraph>;
}  // namespace
REGISTER_IMPL(BfsBased);
REGISTER_IMPL(BfsBasedSep);
REGISTER_IMPL(BfsBasedSmall);

}  // namespace rpo::app::algorithms::edge_orientation
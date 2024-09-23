#include "app/algorithms/edge_orientation/combined.h"

#include "edge_orientation/ds/flip_graph_seperated.h"

namespace rpo::app::algorithms::edge_orientation {
namespace {
using Combined =
    rpo::app::algorithms::Combined<rpo::edge_orientation::ds::StandardFlipGraph>;
using CombinedSep =
    rpo::app::algorithms::Combined<rpo::edge_orientation::ds::StandardSepFlipGraph>;

using DfsMultipleCombined = rpo::app::algorithms::DfsMultipleCombined<
    rpo::edge_orientation::ds::StandardFlipGraph>;
using DfsMultipleCombinedSep = rpo::app::algorithms::DfsMultipleCombined<
    rpo::edge_orientation::ds::StandardSepFlipGraph>;

using DfsMultipleCombinedSmall = rpo::app::algorithms::DfsMultipleCombined<
    rpo::edge_orientation::ds::StandardSmallerGraph>;
}  // namespace
REGISTER_IMPL(Combined);
REGISTER_IMPL(CombinedSep);

REGISTER_IMPL(DfsMultipleCombined);
REGISTER_IMPL(DfsMultipleCombinedSep);
REGISTER_IMPL(DfsMultipleCombinedSmall);

}  // namespace rpo::app::algorithms::edge_orientation
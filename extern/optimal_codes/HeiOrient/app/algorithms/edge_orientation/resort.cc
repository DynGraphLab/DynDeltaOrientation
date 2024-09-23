#include "app/algorithms/edge_orientation/resort.h"

#include "edge_orientation/ds/flip_graph_seperated.h"
#include "edge_orientation/ds/smaller_graph.h"

namespace rpo::app::algorithms::edge_orientation {
namespace {
using Resort =
    rpo::app::algorithms::ResortImpl<rpo::edge_orientation::ds::StandardFlipGraph>;
using ResortSep =
    rpo::app::algorithms::ResortImpl<rpo::edge_orientation::ds::StandardSepFlipGraph>;
using ResortSmall =
    rpo::app::algorithms::ResortImpl<rpo::edge_orientation::ds::StandardSmallerGraph>;
}  // namespace
REGISTER_IMPL(Resort);
REGISTER_IMPL(ResortSep);
REGISTER_IMPL(ResortSmall);

}  // namespace rpo::app::algorithms::edge_orientation
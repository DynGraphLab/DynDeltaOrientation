//
// 

#ifndef DYNAMIC_ALGORITHM_META_2LKHAHEY
#define DYNAMIC_ALGORITHM_META_2LKHAHEY

#include "DynEdgeOrientation.h"
#include "dyn_graph_access.h"
#include "timer.h"

using namespace std;

void run_dynamic_algorithm(std::shared_ptr<dyn_graph_access> & G, 
                           std::vector<std::pair<int, std::pair<NodeID, NodeID> > > &edge_sequence, 
                           std::shared_ptr<dyn_edge_orientation> algorithm ) {

        for (size_t i = 0; i < edge_sequence.size(); ++i) {
                std::pair<NodeID, NodeID> & edge = edge_sequence[i].second;
                if (edge_sequence.at(i).first) {
                        algorithm->handleInsertion(edge.first, edge.second);
                } else {
                        algorithm->handleDeletion(edge.first, edge.second);
                }
        } 

}


#endif /* end of include guard: DYNAMIC_ALGORITHM_META_2LKHAHEY */


#include <memory>

#include "dyn_edge_orientation_BrodalFagerbergCS.h"
#include "dyn_edge_orientation_BFSCS.h"
#include "dyn_edge_orientation_ILP.h"
#include "dyn_edge_orientation_KFlipsCS.h"
#include "dyn_edge_orientation_Naive.h"
#include "dyn_edge_orientation_MaxDecending.h"
#include "dyn_edge_orientation_RWalkCS.h"

//
//

#ifndef DELTA_ORIENTATIONS_ALGORITHM_FACTORY_H
#define DELTA_ORIENTATIONS_ALGORITHM_FACTORY_H

template <typename... Args>
static std::unique_ptr<dyn_edge_orientation> getdyn_edge_orientation_instance(DeltaOrientationsAlgorithmType algoType, Args&&... args) {
    std::unique_ptr<dyn_edge_orientation> solverPtr;
    switch (algoType) {
        case ILP:
            solverPtr = std::make_unique<dyn_edge_orientation_ILP>(std::forward<Args>(args)...);
            break;
        case BFSCS:
            solverPtr = std::make_unique<dyn_edge_orientation_BFSCS>(std::forward<Args>(args)...);
            break;
        case KFLIPS:
            solverPtr = std::make_unique<dyn_edge_orientation_KFlipsCS>(std::forward<Args>(args)...);
            break;
        case RWALKCS:
            solverPtr = std::make_unique<dyn_edge_orientation_RWalkCS>(std::forward<Args>(args)...);
            break;
        case NAIVE:
            solverPtr = std::make_unique<dyn_edge_orientation_Naive>(std::forward<Args>(args)...);
            break;
        case BRODAL_FAGERBERGCS:
            solverPtr = std::make_unique<dyn_edge_orientation_BrodalFagerbergCS>(std::forward<Args>(args)...);
            break;
        case MAXDECENDING:
            solverPtr = std::make_unique<dyn_edge_orientation_MaxDecending>(std::forward<Args>(args)...);
            break;
        case STATIC_ILP:
            solverPtr = std::make_unique<dyn_edge_orientation_ILP>(std::forward<Args>(args)...);
            break;
    }
    return solverPtr;
}

#endif  // DELTA_ORIENTATIONS_ALGORITHM_FACTORY_H

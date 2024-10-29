#include <memory>

#include "dyn_edge_orientation_BFSCS.h"
#include "dyn_edge_orientation_BrodalFagerbergCS.h"
#include "dyn_edge_orientation_ILP.h"
#include "dyn_edge_orientation_KFlipsCS.h"
#include "dyn_edge_orientation_MaxDecending.h"
#include "dyn_edge_orientation_Naive.h"
#include "dyn_edge_orientation_RWalkCS.h"
#include "dyn_edge_orientation_impro_opt.h"
#include "dyn_edge_orientation_improved_opt.h"
#include "dyn_edge_orientation_improved_opt_dfs.h"
#include "dyn_edge_orientation_naive_opt.h"
#include "dyn_edge_orientation_strong_opt.h"
#include "dyn_edge_orientation_strong_opt_dfs.h"

//
//

#ifndef DELTA_ORIENTATIONS_ALGORITHM_FACTORY_H
#define DELTA_ORIENTATIONS_ALGORITHM_FACTORY_H

template <typename... Args>
static std::unique_ptr<dyn_edge_orientation>
getdyn_edge_orientation_instance(DeltaOrientationsAlgorithmType algoType,
                                 Args &&...args) {
  std::unique_ptr<dyn_edge_orientation> solverPtr;
  switch (algoType) {
#ifdef USEILP
  case ILP:
    solverPtr =
        std::make_unique<dyn_edge_orientation_ILP>(std::forward<Args>(args)...);
    break;
#endif
  case BFSCS:
    solverPtr = std::make_unique<dyn_edge_orientation_BFSCS>(
        std::forward<Args>(args)...);
    break;
  case NAIVEOPT:
    solverPtr = std::make_unique<dyn_edge_orientation_naive_opt>(
        std::forward<Args>(args)...);
    break;
  case IMPROOPT:
    solverPtr = std::make_unique<dyn_edge_orientation_impro_opt>(
        std::forward<Args>(args)...);
    break;
  case KFLIPSCS:
    solverPtr = std::make_unique<dyn_edge_orientation_KFlipsCS>(
        std::forward<Args>(args)...);
    break;
  case RWALKCS:
    solverPtr = std::make_unique<dyn_edge_orientation_RWalkCS>(
        std::forward<Args>(args)...);
    break;
  case NAIVE:
    solverPtr = std::make_unique<dyn_edge_orientation_Naive>(
        std::forward<Args>(args)...);
    break;
  case BRODAL_FAGERBERGCS:
    solverPtr = std::make_unique<dyn_edge_orientation_BrodalFagerbergCS>(
        std::forward<Args>(args)...);
    break;
  case MAXDECENDING:
    solverPtr = std::make_unique<dyn_edge_orientation_MaxDecending>(
        std::forward<Args>(args)...);
    break;
#ifdef USEILP
  case STATIC_ILP:
    solverPtr =
        std::make_unique<dyn_edge_orientation_ILP>(std::forward<Args>(args)...);
    break;
#endif
  case STRONG_OPT:
    solverPtr = std::make_unique<dyn_edge_orientation_strong_opt>(
        std::forward<Args>(args)...);
    break;
  case STRONG_OPT_DFS:
    solverPtr = std::make_unique<dyn_edge_orientation_strong_opt_dfs>(
        std::forward<Args>(args)...);
    break;
  case IMPROVED_OPT:
    solverPtr = std::make_unique<dyn_edge_orientation_improved_opt>(
        std::forward<Args>(args)...);
    break;
  case IMPROVED_OPT_DFS:
    solverPtr = std::make_unique<dyn_edge_orientation_improved_opt_dfs>(
        std::forward<Args>(args)...);
  }
  return solverPtr;
}

#endif // DELTA_ORIENTATIONS_ALGORITHM_FACTORY_H
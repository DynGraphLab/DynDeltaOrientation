//
//
#ifdef ILP
#ifndef DELTA_ORIENTATIONS_ILP_H
#define DELTA_ORIENTATIONS_ILP_H

#include <random_functions.h>

#include <algorithm>
#include <iostream>
#include <random>

#include "DeltaOrientationsConfig.h"
#include "graph_access.h"
#include "dyn_graph_access.h"

/**
 * Solves ILP in order to find an orientation with minimum maximum out degree among all possible orientations for G
 * @param orientation found orientation
 * @param G input graph
 * @param config a config with hyper parameters
 */
void solveUsingILP(graph_access& orientation, dyn_graph_access& G, const DeltaOrientationsConfig& config);

/**
 * Solves LP (relaxed ILP) in order to find an orientation with near-minimum maximum out degree among all possible orientations for G
 * @param orientation found orientation
 * @param G input graph
 * @param config a config with hyper parameters
 */
void solveUsingRelaxedILP(graph_access& orientation, dyn_graph_access& G, const DeltaOrientationsConfig& config);


#endif  // DELTA_ORIENTATIONS_ILP_H
#endif  // ILP 

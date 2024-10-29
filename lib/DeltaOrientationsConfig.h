//
//

#ifndef ORIENTATIONS_DELTAORIENTATIONSCONFIG_H
#define ORIENTATIONS_DELTAORIENTATIONSCONFIG_H

#include <string>
#include "definitions.h"

class DeltaOrientationsConfig {
   public:
    bool lsOnDeletion  = false;

    // ilp
    double time_limit = 10000; // time_limit for a single update or in static case: for the whole update
    bool relaxILP     = false;

    // bfs
    unsigned bfsMaxNodes = 10;
    unsigned bfsMaxAlgoAmount = 10;

    // kflips
    unsigned flips = 10;
    bool pruneFlips = false;

    // dfs
    unsigned dfsBranchBound = 10;
    unsigned dfsDepthBound  = 10;
    bool recursiveDFS       = false;
    bool stackDFS           = false;

    // rw
    unsigned rwAmount      = 10;
    unsigned rwLengthBound = 10;
    unsigned bfsDepthBound = 10;

    // Brodal & Fagerberg
    unsigned delta = 0; // maintain delta-orientation

    // num of random queries that we should perform once the orientation is computed
    unsigned num_queries = 0;

    double bfgrowthfactor = 2;

    DeltaOrientationsApplicationStyle applicationStyle = ONCE;
    DeltaOrientationsAlgorithmType algorithmType       = KFLIPSCS;

    int seed = 0;

    std::string output_filename;
};

#endif  // ORIENTATIONS_DELTAORIENTATIONSCONFIG_H

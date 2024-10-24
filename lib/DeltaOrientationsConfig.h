//
//

#ifndef ORIENTATIONS_DELTAORIENTATIONSCONFIG_H
#define ORIENTATIONS_DELTAORIENTATIONSCONFIG_H

#include "definitions.h"
#include <string>

class DeltaOrientationsConfig {
public:
        bool lsOnDeletion = false;

        // ilp
        double time_limit = 10000; // time_limit for a single update or in static
        // case: for the whole update
        bool relaxILP = false;

        // bfs
        unsigned bfsMaxNodes = 10;
        unsigned bfsMaxAlgoAmount = 10;

        // kflips
        unsigned flips = 10;
        bool pruneFlips = false;

        // dfs
        unsigned dfsBranchBound = 10;
        unsigned dfsDepthBound = 10;
        bool recursiveDFS = false;
        bool stackDFS = false;

        // rw
        unsigned rwAmount = 10;
        unsigned rwLengthBound = 10;
        unsigned bfsDepthBound = 10;

        // CCHHQRS
        unsigned b = 10;
        float lambda = 0.1;
        unsigned theta = 0; // 0 or 1

        // Brodal & Fagerberg
        unsigned delta = 0; // maintain delta-orientation

        double bfgrowthfactor = 2;

        DeltaOrientationsApplicationStyle applicationStyle = ONCE;
        DeltaOrientationsAlgorithmType algorithmType = KFLIPS;

        int seed = 0;

        std::string output_filename;
};

#endif // ORIENTATIONS_DELTAORIENTATIONSCONFIG_H

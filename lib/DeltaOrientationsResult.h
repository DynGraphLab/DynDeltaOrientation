//
//

#ifndef DELTA_ORIENTATIONS_DELTAORIENTATIONSRESULT_H
#define DELTA_ORIENTATIONS_DELTAORIENTATIONSRESULT_H

#include "DeltaOrientationsConfig.h"
class DeltaOrientationsResult {

   public:

    double timeElapsed = 0;
    double finalMaxOutDeg = 0;
    double largestMaxOutDeg = 0;
    double applications = 0;

    // how many bfs were actually performed
    double performedBFS = 0;
    double geoMeanVisitedNodes = 0;


};

#endif  // DELTA_ORIENTATIONS_DELTAORIENTATIONSRESULT_H

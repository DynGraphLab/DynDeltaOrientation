/******************************************************************************
 * definitions.h
 *
 * Christian Schulz <christian.schulz.phone@gmail.com>
 * modified by: Jannick Borowitz
 *****************************************************************************/

#ifndef DEFINITIONS_H_JB_DELTA_ORIENTATIONS
#define DEFINITIONS_H_JB_DELTA_ORIENTATIONS

#ifndef NDEBUG
#define DELTAORI_DEBUG(x) x
#define DELTAORI_ASSERT(expression)                                                                                         \
    do {                                                                                                           \
        if (not(expression)) {                                                                                     \
            std::cerr << "ASSERTION FAILED [" << __FILE__ << ":" << __LINE__ << "]. Asserted: " << STR(expression) \
                      << std::endl;                                                                                \
            abort();                                                                                               \
        }                                                                                                          \
    } while (false)
#else
#define DELTAORI_DEBUG(x) \
    do {                  \
    } while (false)
#define DELTAORI_ASSERT(x) \
    do {                   \
    } while (false)
#define DELTAORI_NDEBUG  // no debugging
#endif

#ifndef DELTAORI_IS_LOGGING
#define DELTAORI_IS_LOGGING false
#endif
/**
 * define logging macro
 */
#if DELTAORI_IS_LOGGING == false
#define DELTAORI_LOG(x) \
    do {                \
    } while (false)
#else
#define DELTAORI_LOG(x) std::cout << x << std::endl;
#endif

#include <limits>
#include <queue>
#include <vector>

// use this if you need to handle negative node weights
// or you do subtractions of node weights
typedef signed int SignedNodeWeight;
typedef unsigned int EdgeID;
typedef unsigned int NodeID;
typedef unsigned int NodeWeight;
typedef unsigned int EdgeWeight;
typedef unsigned int PartitionID;
typedef double EdgeRatingType;

struct DynNode {
    EdgeID firstEdge;
};

struct DynEdge {
    NodeID target;
};

/**
 * algorithms
 */
typedef enum { ILP, NAIVEOPT, IMPROOPT, STATIC_ILP,  BFSCS, KFLIPSCS, RWALK, RWALKCS, DFS_PATH, MAXDECENDING, BRODAL_FAGERBERG, BRODAL_FAGERBERGCS, NAIVE,STRONG_OPT, STRONG_OPT_DFS, IMPROVED_OPT, IMPROVED_OPT_DFS} DeltaOrientationsAlgorithmType;

/**
 * ApplicationStyle
 */
typedef enum {ALWAYS, AVGDEG, ONCE} DeltaOrientationsApplicationStyle;

#endif
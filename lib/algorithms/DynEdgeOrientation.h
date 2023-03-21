//
//

#ifndef DELTA_ORIENTATIONS_LOCALSEARCH_H
#define DELTA_ORIENTATIONS_LOCALSEARCH_H

#include <memory>
#include <utility>

#include "DeltaOrientationsConfig.h"
#include "DeltaOrientationsResult.h"
#include "dyn_graph_access.h"

class dyn_edge_orientation {
   public:

    virtual ~dyn_edge_orientation() = default;

    /**
     * Notify local search solver about the edge insertion
     * @param source
     * @param target
     */
    virtual void handleInsertion(NodeID source, NodeID target) = 0;

    /**
     * Notify local search solver about the edge deletion
     * @param source
     * @param target
     */
    virtual void handleDeletion(NodeID source, NodeID target) = 0;

    /**
     * Doing some postprocessing once the overall graph has been read
     */
    virtual void end() = 0;

    virtual bool adjacent(NodeID source, NodeID target) = 0;

    std::shared_ptr<dyn_graph_access> GOrientation;
    DeltaOrientationsConfig config;
    DeltaOrientationsResult& result;

   protected:
    dyn_edge_orientation(std::shared_ptr<dyn_graph_access> GOrientation, DeltaOrientationsConfig config, DeltaOrientationsResult& result)
        : GOrientation(std::move(GOrientation)), config(std::move(config)), result(result) {}

};

#endif  // DELTA_ORIENTATIONS_LOCALSEARCH_H

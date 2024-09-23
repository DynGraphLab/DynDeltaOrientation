/******************************************************************************
 * priority_queue_interface.h 
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Adapted by Jannick Borowitz
 *
 *****************************************************************************/

#ifndef PRIORITY_QUEUE_INTERFACE_20ZSYG7R
#define PRIORITY_QUEUE_INTERFACE_20ZSYG7R

#include "definitions.h"

class priority_queue_interface {
        public:
                priority_queue_interface( ) {};
                virtual ~priority_queue_interface() {};

                /* returns the size of the priority queue */
                virtual NodeID size() = 0;
                virtual bool empty()  = 0 ;
               
                virtual void insert(NodeID id, NodeWeight gain) = 0;
                
                virtual NodeWeight maxValue()     = 0;
                virtual NodeID maxElement() = 0;
                virtual NodeID deleteMax()  = 0;

                virtual void decreaseKey(NodeID node, NodeWeight newNodeWeight) = 0;
                virtual void increaseKey(NodeID node, NodeWeight newKey)  = 0;

                virtual void changeKey(NodeID element, NodeWeight newKey) = 0;
                virtual NodeWeight getKey(NodeID element)  = 0;
                virtual void deleteNode(NodeID node) = 0;
                virtual bool contains(NodeID node)   = 0;
};

typedef priority_queue_interface refinement_pq;

#endif /* end of include guard: PRIORITY_QUEUE_INTERFACE_20ZSYG7R */


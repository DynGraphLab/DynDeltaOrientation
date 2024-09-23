//
// Created by Jannick Borowitz on 08.05.22.
//

#ifndef DELTA_ORIENTATIONS_BUCKETPRIORITYQUEUE_H
#define DELTA_ORIENTATIONS_BUCKETPRIORITYQUEUE_H

#include <iostream>
#include "definitions.h"

class BucketPriorityQueue {
   public:
    using Key = size_t;
    using Value = NodeID;
    using Queue = std::deque<Value>;
    BucketPriorityQueue(size_t init_amount_buckets, size_t elements)
        : buckets(init_amount_buckets), nodesToBuckets(elements, nullptr), max_bucket(0) {}

    void insert(Key key, Value element) {
        if (key > max_bucket) {
            max_bucket = key;
            if(max_bucket >= buckets.size()) buckets.resize(max_bucket*2);
        }
        buckets[key].push_back(element);
        nodesToBuckets[element] = &buckets[key];
    }

    void remove(const Value& element) {
        nodesToBuckets[element]->erase(
            std::remove(nodesToBuckets[element]->begin(), nodesToBuckets[element]->end(), element),
            nodesToBuckets[element]->end());
        nodesToBuckets[element] = nullptr;
        updateMaxBucketKey();
    }

    Value top() { return buckets[max_bucket].front(); }
    void pop() { buckets[max_bucket].pop_front(); }
    Key maxBucket() const { return max_bucket; };

   private:
    std::vector<Queue> buckets;
    std::vector<Queue*> nodesToBuckets;
    Key max_bucket;

    void updateMaxBucketKey() {
        while (buckets[max_bucket].empty() && max_bucket > 0) --max_bucket;
        buckets.resize(max_bucket+1);
    }
};

#endif  // DELTA_ORIENTATIONS_BUCKETPRIORITYQUEUE_H

#ifndef BUCKETS_H
#define BUCKETS_H

#include <bits/stdc++.h>

#include "DeltaOrientationsConfig.h"

using namespace std;    // for my sanity. and readability.

struct DEdge;
struct Vertex;
struct SingleBucket;

class Buckets{
public:
    // contains j, Bj for different j. External list in sorted order, internal list Bj in arbitrary order.
    // Bj contains in neighbours w s.t. j = log(d+(w))
    vector<SingleBucket> buckets;
    int max_bucketID = -1; // needed in delete
    
public:
    DeltaOrientationsConfig config;
    Buckets();
    explicit Buckets(const DeltaOrientationsConfig& config, int n);
    ~Buckets();
    int get_bucket_id(int du) const;
    void update(DEdge *uv);
    void add(DEdge *uv);
    void add_fast(DEdge* uv);
    void remove(DEdge * uv);
};

struct DEdge{
    DEdge* mirror;  // vu
    Vertex *source;
    Vertex *target;
    int count = 0;
    SingleBucket *bucket;              // location of the edge
    list<DEdge*>::iterator location_in_neighbours;
    list<DEdge*>::iterator location_out_neighbours;    // location of edge in the out neighbour list of source
    DEdge(Vertex *u, Vertex *v){source = u, target = v;};
};

struct Vertex{
    NodeID node;
    list<DEdge*> out_edges = list<DEdge*>();
    Buckets in_edges;
    DEdge *self_loop;
    unsigned int out_degree = 0;                // out degree
};

struct SingleBucket{
    int bucketID = -1;
    list<DEdge*> bucket_elements = list<DEdge*>();
    int prev = -1;
    int next = -1;

    SingleBucket(){}
    SingleBucket(unsigned int bucket_ID){bucketID = bucket_ID;}
    ~SingleBucket(){}
};

#endif // BUCKETS_H
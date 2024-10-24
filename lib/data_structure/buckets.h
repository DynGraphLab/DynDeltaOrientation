#ifndef BUCKETS_H
#define BUCKETS_H

#include <bits/stdc++.h>

#include "DeltaOrientationsConfig.h"

// using namespace std; // for my sanity. and readability.

struct DEdge;
struct Vertex;
struct SingleBucket;
class Buckets {
public:
  // contains j, Bj for different j. External list in sorted order, internal
  // list Bj in arbitrary order. Bj contains in neighbours w s.t. j = log(d+(w))
  std::vector<SingleBucket> buckets;
  int max_bucketID = -1; // needed in delete

public:
  Buckets();
  explicit Buckets(const DeltaOrientationsConfig &config, int n);
  ~Buckets();
  int get_bucket_id(int du) const;
  int b;
  void update(DEdge *uv, int outdegree_u);
  void add(DEdge *uv, int out_degree, int bucket_v);
  void add_fast(DEdge *uv);
  void remove(DEdge *uv, std::vector<DEdge *> &);
};

struct DEdge {
  DEdge *mirror; // vu
  // Vertex *source;
  NodeID target;
  int count = 0;
  int bucket; // location of the edge
  int location_in_neighbours;
  int location_out_neighbours; // location of edge in the out neighbour list of
                               // source
  DEdge(NodeID v) { target = v; };
  DEdge() {}
};

struct Vertex {
  std::vector<DEdge *> out_edges;
  Buckets in_edges;
  DEdge *self_loop;
  unsigned int out_degree = 0; // out degree
};

struct SingleBucket {
  int bucketID = -1;
  std::vector<DEdge *> bucket_elements;
  int prev = -1;
  int next = -1;

  SingleBucket() {}
  SingleBucket(unsigned int bucket_ID) { bucketID = bucket_ID; }
  ~SingleBucket() {}
};

#endif // BUCKETS_H
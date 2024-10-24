#include "buckets.h"
#include <vector>

Buckets::Buckets() {}
double lambda_precomp;
Buckets::Buckets(const DeltaOrientationsConfig &config, int n) {
  // precompute lambda term, not needed to recompute everytime
  // also do not store the config object in Bucket class
  lambda_precomp = 1.0 / (log(1 + config.lambda));
  b = config.b;
  // buckets.resize(get_bucket_id(config.b * n) + 2); // do not allocate all
  // memory
}

Buckets::~Buckets() = default;

int Buckets::get_bucket_id(const int du) const{
  float f = b * 4;
  float duf = du;
  return static_cast<int>(log(std::max(duf, f)) * lambda_precomp);
}

void Buckets::add(DEdge *uv, int out_degree, int bucket_v) {
  int j = get_bucket_id(out_degree);
  if (j >= buckets.size()) {
    buckets.resize(j + 1);
    // std::cout << buckets.size() << std::endl;
  }

  buckets[j].bucket_elements.push_back(uv);
  uv->location_in_neighbours = buckets[j].bucket_elements.size() - 1;
  uv->bucket = j;

  // The bucket already exists (prev and next are up to date)
  if (buckets[j].bucketID == j) {
    return;
  }

  // The bucket was unused - prev and next have to be set
  buckets[j].bucketID = j;

  // Check if j is the new max bucket
  if (max_bucketID == -1) {
    max_bucketID = j;
  } else if (j > max_bucketID) {
    buckets[max_bucketID].next = j;
    buckets[j].prev = max_bucketID;
    max_bucketID = j;
  }
  // Or insert
  else {
    int j_self = bucket_v;
    int pos = max_bucketID;
    int pos2 = -1;
    if (j <= j_self) {
      pos = j_self;
      pos2 = buckets[j_self].next;
    } // the position will be very close to j in the cases where the insertion
      // has to be fast
    while (pos > j) {
      pos2 = pos;
      pos = buckets[pos].prev;
    }
    if (pos > -1) { // j is not the lowest bucket
      buckets[pos].next = j;
    }
    buckets[j].prev = pos;
    buckets[j].next = pos2;
    buckets[pos2].prev = j;
  }
}

// update the element after incrementing the out degree of the source
void Buckets::update(DEdge *uv, int outdegree_u) {
  int j = get_bucket_id(outdegree_u);
  int j_prev = uv->bucket;
  if (j == j_prev) {
    return;
  }
  if (j >= buckets.size()) {
    buckets.resize(j + 1);
  }
  if (j > max_bucketID) {
    max_bucketID = j;
  }

  if (buckets[j].bucketID ==
      -1) { // bucket j does not exist yet. We will first insert it and then
            // update the edges, possibly removing j_prev.
    if (j == j_prev - 1) {
      int p = buckets[j_prev].prev;
      if (p != -1) { // j_prev was not the lowest bucket
        buckets[p].next = j;
      }
      buckets[j].prev = p;
      buckets[j_prev].prev = j;
      buckets[j].next = j_prev;
    } else if (j == j_prev + 1) {
      int n = buckets[j_prev].next;
      if (n != -1) { // j_prev was not the highest bucket
        buckets[n].prev = j;
      }
      buckets[j].next = n;
      buckets[j_prev].next = j;
      buckets[j].prev = j_prev;
    } else {
      std::cerr << "update leads to a non consecutive bucket";
    }
  }

  remove(uv, buckets[uv->bucket].bucket_elements);

  buckets[j].bucketID = j;
  buckets[j].bucket_elements.push_back(uv);
  uv->location_in_neighbours = (buckets[j].bucket_elements.size() - 1);
  uv->bucket = j;
}

void Buckets::remove(DEdge *uv,
                     std::vector<DEdge *> &uv_bucket_bucket_elements) {
  // uv->target->in_edges.buckets[uv->bucket]
  // uv->bucket->bucket_elements.erase(uv->location_in_neighbours);
  if (uv_bucket_bucket_elements.size() > 1) {
    std::swap(uv_bucket_bucket_elements[uv->location_in_neighbours],
              uv_bucket_bucket_elements[uv_bucket_bucket_elements.size() - 1]);
    uv_bucket_bucket_elements[uv->location_in_neighbours]
        ->location_in_neighbours = uv->location_in_neighbours;
  }
  uv_bucket_bucket_elements.resize(uv_bucket_bucket_elements.size() - 1);

  uv->location_in_neighbours = uv_bucket_bucket_elements.size();

  int j = uv->bucket;

  if (uv_bucket_bucket_elements.empty()) {
    int p = buckets[j].prev;
    int n = buckets[j].next;
    if (n != -1) { // j is not max bucket
      buckets[n].prev = buckets[j].prev;
    }
    if (p != -1) { // j is not lowest bucket
      buckets[p].next = buckets[j].next;
    }
    if (j ==
        max_bucketID) { // because of the self loop, there is always a bucket
      max_bucketID = buckets[max_bucketID].prev;
    }

    buckets[j].bucketID = -1;
    buckets[j].next = -1;
    buckets[j].prev = -1;
  }
}

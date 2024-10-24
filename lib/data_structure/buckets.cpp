#include "buckets.h"

Buckets::Buckets() {}

Buckets::Buckets(const DeltaOrientationsConfig &config, int n) {
  this->config = config;
  buckets.resize(get_bucket_id(config.b * n) + 2);
}

Buckets::~Buckets() = default;

int Buckets::get_bucket_id(const int du) const {
  return static_cast<int>(log(du) / log(1 + config.lambda));
}

void Buckets::add(DEdge *uv) {
  int j = get_bucket_id(uv->source->out_degree);

  buckets[j].bucket_elements.push_back(uv);
  uv->location_in_neighbours = prev(buckets[j].bucket_elements.end());
  uv->bucket = &buckets[j];

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
    int j_self = uv->target->self_loop->bucket->bucketID;
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
void Buckets::update(DEdge *uv) {
  int j = get_bucket_id(uv->source->out_degree);
  int j_prev = uv->bucket->bucketID;
  if (j == j_prev) {
    return;
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
    } else if (j = j_prev + 1) {
      int n = buckets[j_prev].next;
      if (n != -1) { // j_prev was not the highest bucket
        buckets[n].prev = j;
      }
      buckets[j].next = n;
      buckets[j_prev].next = j;
      buckets[j].prev = j_prev;
    } else {
      cerr << "update leads to a non consecutive bucket";
    }
  }

  remove(uv);

  buckets[j].bucketID = j;
  buckets[j].bucket_elements.push_back(uv);
  uv->location_in_neighbours = prev(buckets[j].bucket_elements.end());
  uv->bucket = &buckets[j];
}

void Buckets::remove(DEdge *uv) {
  uv->bucket->bucket_elements.erase(uv->location_in_neighbours);
  uv->location_in_neighbours = uv->bucket->bucket_elements.end();

  int j = uv->bucket->bucketID;

  if (uv->bucket->bucket_elements.empty()) {
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

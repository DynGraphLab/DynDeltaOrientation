/******************************************************************************
 * bucket_pq.h
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Adapted by Jannick Borowitz
 *
 *****************************************************************************/

#ifndef BUCKET_PQ_EM8YJPA9
#define BUCKET_PQ_EM8YJPA9

#include <limits>
#include <unordered_map>

#include "priority_queue_interface.h"

class bucket_pq : public priority_queue_interface {
   public:
    bucket_pq(const NodeWeight& buckets_input);

    virtual ~bucket_pq(){};

    NodeID size();
    void insert(NodeID id, NodeWeight gain);
    bool empty();

    NodeWeight maxValue();
    NodeWeight minValue();
    NodeID maxElement();
    NodeID deleteMax();

    void decreaseKey(NodeID node, NodeWeight newNodeWeight);
    void increaseKey(NodeID node, NodeWeight newNodeWeight);

    void changeKey(NodeID element, NodeWeight newKey);
    NodeWeight getKey(NodeID element);
    void deleteNode(NodeID node);

    bool contains(NodeID node);

    const std::vector<NodeID>& getBucketByKey(NodeWeight key) const;

   private:
    using Count = size_t;

    NodeID m_elements;
    NodeWeight m_buckets_count;
    unsigned m_max_idx;  // points to the non-empty bucket with the largest gain
    unsigned m_min_idx;  // points to the non-empty bucket with the smallest gain

    std::unordered_map<NodeID, std::pair<Count, NodeWeight> > m_queue_index;
    std::vector<std::vector<NodeID> > m_buckets;
};

inline bucket_pq::bucket_pq(const NodeWeight& buckets_input) {
    m_elements = 0;
    m_buckets_count = buckets_input;
    m_max_idx = 0;
    m_min_idx = 0;

    m_buckets.resize(m_buckets_count);
}

inline NodeID bucket_pq::size() { return m_elements; }

inline void bucket_pq::insert(NodeID node, NodeWeight gain) {
    unsigned address = gain;
    if (address >= m_buckets_count) {
        m_buckets_count = address + 1;
        m_buckets.resize(m_buckets_count);
    }
    if (address > m_max_idx) {
        m_max_idx = address;
    }

    if (address < m_min_idx) {
        m_min_idx = address;
    }

    m_buckets[address].push_back(node);
    m_queue_index[node].first = m_buckets[address].size() - 1;  // store position
    m_queue_index[node].second = gain;

    m_elements++;
}

inline bool bucket_pq::empty() { return m_elements == 0; }

inline NodeWeight bucket_pq::maxValue() { return m_max_idx; }

inline NodeWeight bucket_pq::minValue() { return m_min_idx; }

inline NodeID bucket_pq::maxElement() { return m_buckets[m_max_idx].back(); }

inline NodeID bucket_pq::deleteMax() {
    NodeID node = m_buckets[m_max_idx].back();
    m_buckets[m_max_idx].pop_back();
    m_queue_index.erase(node);

    if (m_buckets[m_max_idx].size() == 0) {
        // update max_idx
        while (m_max_idx != 0) {
            m_max_idx--;
            if (m_buckets[m_max_idx].size() > 0) {
                break;
            }
        }
    }

    m_elements--;
    return node;
}

inline void bucket_pq::decreaseKey(NodeID node, NodeWeight new_gain) { changeKey(node, new_gain); }

inline void bucket_pq::increaseKey(NodeID node, NodeWeight new_gain) { changeKey(node, new_gain); }

inline NodeWeight bucket_pq::getKey(NodeID node) { return m_queue_index[node].second; }

inline void bucket_pq::changeKey(NodeID node, NodeWeight new_gain) {
    deleteNode(node);
    insert(node, new_gain);
}

inline void bucket_pq::deleteNode(NodeID node) {
    ASSERT_TRUE(m_queue_index.find(node) != m_queue_index.end());
    Count in_bucket_idx = m_queue_index[node].first;
    NodeWeight old_gain = m_queue_index[node].second;
    unsigned address = old_gain;

    if (m_buckets[address].size() > 1) {
        // swap current element with last element and pop_back
        m_queue_index[m_buckets[address].back()].first = in_bucket_idx;  // update helper structure
        std::swap(m_buckets[address][in_bucket_idx], m_buckets[address].back());
        m_buckets[address].pop_back();
    } else {
        // size is 1
        m_buckets[address].pop_back();
        if (address == m_max_idx) {
            // update max_idx
            while (m_max_idx != 0) {
                --m_max_idx;
                if (!m_buckets[m_max_idx].empty()) {
                    break;
                }
            }
        }
        if (address == m_min_idx) {
            // update min_idx
            if(m_max_idx == 0) {
                m_min_idx = 0;
            }else {
                ASSERT_TRUE(m_min_idx <= m_max_idx);
                while (m_min_idx != m_max_idx) {
                    ++m_min_idx;
                    if(!m_buckets[m_min_idx].empty()) {
                        break;
                    }
                }
            }
        }
    }

    m_elements--;
    m_queue_index.erase(node);
}

inline bool bucket_pq::contains(NodeID node) { return m_queue_index.find(node) != m_queue_index.end(); }

inline const std::vector<NodeID>& bucket_pq::getBucketByKey(NodeWeight key) const {
    ASSERT_TRUE(key <= m_max_idx);
    ASSERT_TRUE(key >= m_min_idx);
    return m_buckets[key];
}

#endif /* end of include guard: BUCKET_PQ_EM8YJPA9 */

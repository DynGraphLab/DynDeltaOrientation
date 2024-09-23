#pragma once

#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include "utils/range.h"

namespace rpo {
namespace ds {
namespace {
using rpo::utils::Range;
}
/// @brief , Max value can not be zero
/// @tparam IndexType
/// @tparam ValueType
template <typename IndexType = size_t, typename ValueType = size_t>
class FullAdressableDecreasingPriorityQueue {
  /// @brief contains the offset to each level, note that sorting is decreasing
  std::vector<IndexType> levels_offset, dead_count, max_volume, dimpr, simpr;
  IndexType _high_level_hint, _lowest_level_hint;
  /// @brief contains the values sorted from high to low
  std::vector<IndexType> queue;
  // allows to directly find a value
  std::vector<IndexType> position_in_queue;  // maybe not needed if willing to search
  std::vector<ValueType>& values;

 public:
  /// @brief Initializes the data structure with the values given.
  /// @param _values
  FullAdressableDecreasingPriorityQueue(std::vector<ValueType>& _values, bool init = true)
      : queue(_values.size()), position_in_queue(_values.size()), values(_values) {
    if (init) {
      resort();
    }
  }
  void resort() {
    std::iota(queue.begin(), queue.end(), 0);
    std::sort(queue.begin(), queue.end(), [&](auto a, auto b) { return values[a] > values[b]; });
    IndexType offset = 0;
    ValueType curr_level = values[queue[0]];
    _high_level_hint = curr_level;  // init
    _lowest_level_hint = 0;
    levels_offset.resize(curr_level + 1);
    dead_count.resize(curr_level + 1, 0);
    max_volume.resize(curr_level + 1, 0);
    dimpr.resize(curr_level + 1, 0);
    simpr.resize(curr_level + 1, 0);

    for (auto p : queue) {
      // check if update
      if (values[p] < curr_level) {
        while (curr_level > values[p]) {  // while loop since value may not be contigous
          curr_level--;
          levels_offset[curr_level] = offset;
        }
      }
      position_in_queue[p] = offset++;
      assert(values[p] <= curr_level);
    }
    while (curr_level > 0) {  // while loop since value may not be contigous
      curr_level--;
      levels_offset[curr_level] = offset;
    }
  }
  IndexType& dead(ValueType level) { return dead_count[level]; }
  IndexType& maxvolume(ValueType level) { return max_volume[level]; }
  IndexType& directimprovement(ValueType level) { return dimpr[level]; }
  IndexType& secondimprovement(ValueType level) { return simpr[level]; }

  IndexType size(ValueType level) {
    if (level == 0) {
      return queue.size() - levels_offset[0];
    }
    return levels_offset[level - 1] - levels_offset[level];
  }
  /// @brief Return a highest level item(cache e)
  /// @return Returns one of the highest_level items
  auto highest_level_item() const { return queue[0]; }
  auto highest_non_empty_level() {
    while (size(_high_level_hint) == 0) {
      _high_level_hint--;
    }
    return _high_level_hint;
  }
  auto lowest_non_empty_level() {
    while (size(_lowest_level_hint) == 0) {
      _lowest_level_hint++;
    }
    return _lowest_level_hint;
  }
  bool empty() { return queue.empty(); }
  auto lowest_level_item() const { return queue.back(); }
  auto lowest_level_items() {
    return Range(queue.rbegin(), queue.rend() - levels_offset[highest_non_empty_level() - 2]);
  }
  auto highest_level_items() {
    return Range(queue.begin(), queue.begin() + levels_offset[highest_non_empty_level() - 1]);
  }
  auto offset(ValueType level) { return levels_offset[level]; }

  /// @brief pushes one value from high to low, does not update value, value needs to be update
  /// AFTERWARDS
  /// @param high the id of the high value
  /// @param low
  void push_one_value_down_non_modifying(IndexType high, IndexType low) {
    const auto high_v = values[high];
    const auto low_v = values[low];
    assert(high_v > low_v);
    // update at lower
    const auto low_boundary_item = queue[levels_offset[low_v]];
    auto pos = position_in_queue[low];
    assert(levels_offset[low_v] <= pos);
    // swap to boundary position by swaping values and swaping the position
    std::swap(queue[position_in_queue[low]], queue[levels_offset[low_v]]);
    std::swap(position_in_queue[low], position_in_queue[low_boundary_item]);
    // update boundary at lower, now one element less
    levels_offset[low_v]++;

    // update at higher
    IndexType last_higher_item_offset = levels_offset[high_v - 1] - 1;
    const auto high_boundary_item = queue[last_higher_item_offset];
    assert(position_in_queue[high] <= last_higher_item_offset);
    // swap to boundary position by swaping values and swaping the position
    std::swap(queue[position_in_queue[high]], queue[last_higher_item_offset]);
    std::swap(position_in_queue[high], position_in_queue[high_boundary_item]);
    // update levels to gain one element
    levels_offset[high_v - 1]--;
    // update values
  }
  void push_one_value_down(IndexType high, IndexType low) {
    push_one_value_down_non_modifying(high, low);
    values[high]--;
    values[low]++;
  }
  void push_down_non_modifying(IndexType high) {
    const auto high_v = values[high];
    // update at higher
    IndexType last_higher_item_offset = levels_offset[high_v - 1] - 1;
    const auto high_boundary_item = queue[last_higher_item_offset];
    assert(position_in_queue[high] <= last_higher_item_offset);
    // swap to boundary position by swaping values and swaping the position
    std::swap(queue[position_in_queue[high]], queue[last_higher_item_offset]);
    std::swap(position_in_queue[high], position_in_queue[high_boundary_item]);
    // update levels to gain one element
    levels_offset[high_v - 1]--;
  }
  void push_down(IndexType high) {
    push_down_non_modifying(high);
    values[high]--;
    if (values[high] == 0) {
      queue.pop_back();
    }
  }
  void pop_back() { queue.pop_back(); }

  IndexType queue_item(IndexType i) { return queue[i]; }
  ValueType level(IndexType i) const { return values[i]; }
  void printOffsets() {
    for (auto c : levels_offset) {
      std::cout << c << std::endl;
    }
  }
  void printSizes() {
    size_t v = 0;
    for (auto c : levels_offset) {
      if (size(v) != 0) {
        std::cout << v << ":" << size(v) << " d:" << dead(v) << " M:" << maxvolume(v) << std::endl;
      }
      v++;
    }
  }
  auto& vals() { return values; }
};
}  // namespace ds

}  // namespace rpo

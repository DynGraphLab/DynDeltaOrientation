
#include "ds/addressable_priority_queue.h"

#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace {
using FADPQ = rpo::ds::FullAdressableDecreasingPriorityQueue<size_t, size_t>;
}  // namespace

TEST(FullAdressableDecreasingPriorityQueueTest, Initialization) {
  std::vector<size_t> values = {1, 7, 5, 5, 3};
  FADPQ queue(values);
  EXPECT_EQ(queue.size(7), 1);
  EXPECT_EQ(queue.size(6), 0);
  EXPECT_EQ(queue.size(5), 2);
  EXPECT_EQ(queue.size(4), 0);
  EXPECT_EQ(queue.size(3), 1);
  EXPECT_EQ(queue.size(2), 0);
  EXPECT_EQ(queue.size(1), 1);

  EXPECT_EQ(queue.highest_level_item(), 1);

  EXPECT_EQ(queue.highest_non_empty_level(), 7);
}
TEST(FullAdressableDecreasingPriorityQueueTest, Decrease) {
  std::vector<size_t> values = {1, 7, 5, 5, 3};
  FADPQ queue(values);

  EXPECT_EQ(queue.highest_level_item(), 1);
  EXPECT_EQ(queue.highest_non_empty_level(), 7);
  queue.push_one_value_down_non_modifying(1, 4);
  values[1]--;
  values[4]++;

  EXPECT_EQ(queue.highest_level_item(), 1);
  EXPECT_EQ(queue.highest_non_empty_level(), 6);

  queue.push_one_value_down_non_modifying(1, 4);
  values[1]--;
  values[4]++;
  EXPECT_EQ(queue.size(5), 4);
  EXPECT_EQ(queue.highest_non_empty_level(), 5);
}
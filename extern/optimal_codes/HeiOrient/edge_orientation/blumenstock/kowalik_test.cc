#include "edge_orientation/blumenstock/kowalik.h"

#include <sstream>

#include "edge_orientation/ds/flow_graph.h"
#include "gtest/gtest.h"
#include "io/graph_reader.h"
namespace {
using BlumenstockPushRelabelDS =
    rpo::edge_orientation::blumenstock::BlumenstockPushRelabelDS<>;
}  // namespace

TEST(BlumenstockTest, SimpleGraph) {
  std::stringstream test_instance_graph(
      "6 5\n"
      "2 3\n"
      "1 3 4\n"
      "1 2 4\n"
      "2 3\n"
      "2\n"
      "5\n");
  auto res = rpo::io::readGraph<BlumenstockPushRelabelDS>(test_instance_graph, false, true);
  EXPECT_EQ(res.status(), absl::OkStatus());
  auto instance = std::move(res.value());
  instance->solve_repeat();
  EXPECT_EQ(instance->maxOutdegree(), 2);
}
TEST(BlumenstockTest, SimplestGraph) {
  std::stringstream test_instance_graph(
      "4 5\n"
      "2 3\n"
      "1 3 4\n"
      "1 2 4\n"
      "2 3\n");
  auto res = rpo::io::readGraph<BlumenstockPushRelabelDS>(test_instance_graph, false, true);
  EXPECT_EQ(res.status(), absl::OkStatus());
  auto instance = std::move(res.value());
  instance->solve_repeat();
  EXPECT_EQ(instance->maxOutdegree(), 2);
}

TEST(BlumenstockTest, SimplerGraph) {
  std::stringstream test_instance_graph(
      "3 2\n"
      "2 3\n"
      "1\n"
      "1\n");
  auto res = rpo::io::readGraph<BlumenstockPushRelabelDS>(test_instance_graph, false, true);
  EXPECT_EQ(res.status(), absl::OkStatus());
  auto instance = std::move(res.value());
  instance->solve_repeat();
  EXPECT_EQ(instance->maxOutdegree(), 1);
}
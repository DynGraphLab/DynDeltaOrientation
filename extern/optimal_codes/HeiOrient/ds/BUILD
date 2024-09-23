load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "empty_problem",
    hdrs = [
        "empty_problem.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//utils:logging",
        "//utils:random_lib",
        "//utils:range",
        "@com_google_absl//absl/strings",
    ],
)



cc_library(
    name = "edge_orientation",
    hdrs = ["edge_orientation.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "undirected_graph",
    hdrs = ["undirected_graph.h"],
    visibility = ["//visibility:public"],
    deps = ["//utils:range"],
)

cc_library(
    name = "addressable_priority_queue",
    hdrs = ["addressable_priority_queue.h"],
    visibility = ["//visibility:public"],
    deps = [
    ],
)

cc_test(
    name = "addressable_priority_queue_test",
    srcs = ["addressable_priority_queue_test.cc"],
    deps = [
        ":addressable_priority_queue",
        "//utils:range",
        "@com_google_googletest//:gtest_main",
    ],
)

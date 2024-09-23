load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "kowalik",
    hdrs = [
        "kowalik.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//edge_orientation/ds",
        "//edge_orientation/flows:dinic",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "kowalik_test",
    size = "small",
    srcs = ["kowalik_test.cc"],
    deps = [
        "kowalik",
        "//ds:modifiable_hypergraph_lib",
        "//edge_orientation/ds",
        "//edge_orientation/flows:dinic",
        "//io:graph_reader",
        "@com_google_googletest//:gtest_main",
    ],
)

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ds",
    hdrs = [
        "flip_graph.h",
        "flip_graph_seperated.h",
        "flow_graph.h",
        "smaller_graph.h",
    ],
    defines = select({
        "//:avx": [
            "AVX_ENABLED_DEFINED",
        ],
        "//conditions:default": [
        ],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//utils:logging",
        "//utils:range",
        "//utils:strong_type",
    ],
)

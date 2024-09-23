load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ilp",
    hdrs = [
        "ilp.h",
    ],
    defines = select({
        "//:gurobi": [
            "USE_GUROBI_ENABLED",
        ],
        "//conditions:default": [
        ],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/status",
    ] + select({
        "//:gurobi": [
            "@grb//:gurobi_cxx",
        ],
        "//conditions:default": [
        ],
    }) + select({
        "//:scip": [
            "@scip",
            "@scippp",
        ],
        "//conditions:default": [
        ],
    }) + select({
        "//:spack": ["@soplex"],
        "//conditions:default": [],
    }),
)

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")


cc_library(
    name = "graph_reader",
    hdrs = ["graph_reader.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//utils:logging",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "binary_reader",
    hdrs = [
        "binary_reader.h",
        "binary_utils.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//utils:logging",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_binary(
    name = "mtx_to_unihypergraph",
    srcs = ["mtx_to_unihypergraph.cc"],
    deps = [
        ":hgr_writer",
        ":mtx_reader",
    ],
)






cc_library(
    name = "mtx_reader",
    hdrs = ["mtx_reader.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//utils:logging",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)


cc_library(
    name = "seq_reader",
    hdrs = ["seq_reader.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//utils:logging",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)


cc_binary(
    name = "mtx_to_enc",
    srcs = ["mtx_to_enc.cc"],
    deps = [
        "binary_reader",
        "binary_writer",
        "//io:mtx_reader",
    ],
)

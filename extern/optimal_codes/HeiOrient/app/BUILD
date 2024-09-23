load("@com_google_protobuf//:protobuf.bzl", "py_proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_binary(
    name = "app",
    srcs = [
        "app.cc",
        "//:build-info.h",
    ],
    malloc = select({
        "//:gperftools_tcmalloc": "@gperftools",
        "//conditions:default": "@bazel_tools//tools/cpp:malloc",
    }),
    deps = [
        ":app_io_cc_proto",
        "//app/algorithms/edge_orientation",
        "//utils:logging",
        "//utils:random_lib",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_proto_library(
    name = "app_io_cc_proto",
    visibility = ["//visibility:public"],
    deps = [":app_io_proto"],
)

proto_library(
    name = "app_io_proto",
    srcs = ["app_io.proto"],
    deps = [
        "@com_google_protobuf//:duration_proto",
        "@com_google_protobuf//:timestamp_proto",
    ],
)

py_proto_library(
    name = "app_io_py_proto",
    srcs = ["app_io.proto"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_protobuf//:protobuf_python",
    ],
)

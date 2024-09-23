load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "algorithm_base_impl_lib",
    srcs = ["algorithm_impl.cc"],
    hdrs = [
        "algorithm_impl.h",
        "//:build-info.h",
    ],
    defines = select({
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//app:app_io_cc_proto",
        "//io:binary_reader",
        "//io:graph_reader",
        "//io:mtx_reader",
        "//io:seq_reader",
        "//utils:systeminfo",
        "@com_google_absl//absl/status:statusor",
    ],
)

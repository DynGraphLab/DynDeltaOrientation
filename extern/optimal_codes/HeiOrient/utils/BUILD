load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "random_lib",
    hdrs = ["random.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "strong_type",
    hdrs = ["strong_type.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "range",
    hdrs = ["range.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "systeminfo",
    srcs = ["systeminfo.cc"],
    hdrs = ["systeminfo.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "logging",
    hdrs = ["logging.h"],
    defines = select({
        "//:logging": ["LOGGING_ENABLED_T"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
)

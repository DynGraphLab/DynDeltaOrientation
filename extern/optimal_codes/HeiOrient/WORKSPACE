load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//utils/workspace/gurobi:gurobi.bzl", "gurobi_home")

gurobi_home(name = "grb")

http_archive(
    name = "rules_proto",
    sha256 = "80d3a4ec17354cccc898bfe32118edd934f851b03029d63ef3fc7c8663a7415c",
    strip_prefix = "rules_proto-5.3.0-21.5",
    urls = [
        "https://github.com/bazelbuild/rules_proto/archive/refs/tags/5.3.0-21.5.tar.gz",
    ],
)

http_archive(
    name = "com_google_absl",
    strip_prefix = "abseil-cpp-b971ac5250ea8de900eae9f95e06548d14cd95fe",
    urls = ["https://github.com/abseil/abseil-cpp/archive/b971ac5250ea8de900eae9f95e06548d14cd95fe.zip"],
)

http_archive(
    name = "bazel_skylib",
    sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
    urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz"],
)

http_archive(
    name = "com_google_googletest",
    sha256 = "ab78fa3f912d44d38b785ec011a25f26512aaedc5291f51f3807c592b506d33a",
    strip_prefix = "googletest-58d77fa8070e8cec2dc1ed015d66b454c8d78850",
    urls = ["https://github.com/google/googletest/archive/58d77fa8070e8cec2dc1ed015d66b454c8d78850.zip"],
)

http_archive(
    name = "rules_foreign_cc",
    # TODO: Get the latest sha256 value from a bazel debug message or the latest
    #       release on the releases page: https://github.com/bazelbuild/rules_foreign_cc/releases
    #
    # sha256 = "...",
    strip_prefix = "rules_foreign_cc-8d540605805fb69e24c6bf5dc885b0403d74746a",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/8d540605805fb69e24c6bf5dc885b0403d74746a.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf",
    tag = "v3.20.3",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

http_archive(
    name = "ncurses",
    build_file = "//utils/workspace:BUILD.ncurses",
    sha256 = "57614210c8d4fd3464805eb4949f5d9d255d6248a8590ae446248c28f5facb7f",
    strip_prefix = "ncurses-6.4",
    urls = ["https://github.com/mirror/ncurses/archive/refs/tags/v6.4.zip"],
)

new_git_repository(
    name = "gperftools",
    build_file = "//utils/workspace:BUILD.gperftools",
    commit = "21277bb30b0c34b0d717f82484028d1a6d6d4322",
    remote = "https://github.com/rpo/gperftools.git",
)

new_git_repository(
    name = "scip",
    build_file = "//utils/workspace:BUILD.scip",
    commit = "e1ce3e22badba6c0135968d98f232e8b7a15ad8d",
    patch_args = ["-p1"],
    patches = ["//utils/workspace:scip.patch"],
    remote = "https://github.com/scipopt/scip",
)

new_local_repository(
    name = "libncurses",
    build_file = "//utils/workspace:BUILD.libncurses",
    path = ".spack-env/view/",
)

#python_configure(name = "local_config_python")

new_git_repository(
    name = "matplotlibcpp17",
    build_file = "//utils/workspace:BUILD.matplotlibcpp17",
    commit = "779f108f192a27761cae3724929850fe65de625c",
    remote = "https://github.com/soblin/matplotlibcpp17.git",
)

http_archive(
    name = "pybind11",
    build_file = "//utils/workspace:BUILD.pybind11",
    sha256 = "d475978da0cdc2d43b73f30910786759d593a9d8ee05b1b6846d1eb16c6d2e0c",
    strip_prefix = "pybind11-2.11.1",
    urls = [
        #      "https://mirror.bazel.build/github.com/pybind/pybind11/archive/v2.11.1.tar.gz",
        "https://github.com/pybind/pybind11/archive/v2.11.1.tar.gz",
    ],
)

new_local_repository(
    name = "spack_python",
    build_file = "//utils/workspace:BUILD.spack_python",
    path = ".spack-env/view/",
)

# Hedron's Compile Commands Extractor for Bazel
# https://github.com/hedronvision/bazel-compile-commands-extractor
http_archive(
    name = "hedron_compile_commands",
    strip_prefix = "bazel-compile-commands-extractor-daae6f40adfa5fdb7c89684cbe4d88b691c63b2d",

    # Replace the commit hash (daae6f40adfa5fdb7c89684cbe4d88b691c63b2d) in both places (below) with the latest (https://github.com/hedronvision/bazel-compile-commands-extractor/commits/main), rather than using the stale one here.
    # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
    url = "https://github.com/hedronvision/bazel-compile-commands-extractor/archive/daae6f40adfa5fdb7c89684cbe4d88b691c63b2d.tar.gz",
    # When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'hedron_compile_commands' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
)

load("@hedron_compile_commands//:workspace_setup.bzl", "hedron_compile_commands_setup")

hedron_compile_commands_setup()

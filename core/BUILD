load("@rules_cc//cc:defs.bzl", "cc_library")
filegroup(
    name = "core_for_cpplint",
    srcs = glob(["*.cpp","*.cc"]),
    visibility = ["//visibility:public"],
)
cc_library(
    name = "core",
    visibility = ["//visibility:public"],
    deps = [
        "//core/common:common_converter",
        "//core/common:common_types",
    ],
)

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
        "//core/application:simba_application",
        "//core/common:common_converter",
        "//core/common:common_types",
        "//core/logger:console_logger",
        "//core/results:results_lib",
        "//core/logger:Logger",
    ],
)

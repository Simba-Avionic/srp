cc_test(
    name = "core_test",
    srcs = [
        "application.cc",
        "common.cc",
        "logger.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/application:simba_application",
        "//core/common:common_converter",
        "//core/common:common_types",
        "//core/logger:console_logger",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_binary(
    name = "core_benchmarks",
    srcs = ["benchmark/sample_benchmark.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/application:simba_application",
        "@com_google_benchmark//:benchmark_main",
    ],
)

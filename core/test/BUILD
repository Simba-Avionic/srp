


cc_test(
    name = "core_test",
    srcs = ["application.cc","common.cc","logger.cc"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/logger:console_logger",
        "//core/application:simba_application",
        "//core/common:common_converter",
        "//core/common:common_types",
        ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "core_benchmarks",
    srcs = ["benchmark/sample_benchmark.cc"],
    deps = [
        "@com_google_benchmark//:benchmark_main",
        "//core/application:simba_application",

    ],
    visibility = ["//visibility:public"],
)
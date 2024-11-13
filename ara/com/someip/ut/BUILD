cc_test(
    name = "structure_test",
    srcs = ["header_structure_test.cc"],
    deps = [
        "//ara/com/someip:someip_lib",
        "@com_google_googletest//:gtest_main",
    ],
    size = "medium",
)

cc_test(
    name = "benchmarks",
    srcs = ["sample_benchmark.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com/someip:someip_lib",
        "//communication-core/someip:bench",
        "//communication-core/someip/factory:bench1",
        "//communication-core/someip/factory:bench2",
        "@com_google_benchmark//:benchmark_main",
    ],
    size = "medium",
)

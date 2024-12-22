cc_test(
    name = "nmea_test",
    srcs = [
        "nmea_tests.cc"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/gps:gps_data_parser",
    ],
)
cc_test(
    name = "benchmarks",
    srcs = ["nmea_benchmark.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/gps:gps_data_parser",
        "@com_google_benchmark//:benchmark_main",
    ],
)
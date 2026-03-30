cc_test(
    name = "apogee_detector_tester",
    srcs = [
        "apogee_detector_test.cc",
        "DataLoader.cpp",
        "DataLoader.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/apogee:apogee_lib",
        "@bazel_tools//tools/cpp/runfiles", 
        "//core/apogee/ut/csvReader:csvReader",
    ],
    data = ["@simulation_data_apogee//:simulation_data"], 
)

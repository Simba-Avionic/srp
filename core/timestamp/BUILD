cc_library(
    name = "timestamp_controller",
    hdrs = [
        "timestamp_driver.hpp",
        "Itimestamp_driver.h",
        ],
    srcs = ["timestamp_driver.cpp"],
    visibility = ["//visibility:public"],
    deps = [
        "@srp_platform//bindings/common/shm:shm_lib",
        "@srp_platform//ara/core:instance_specifier",
        "@srp_platform//ara/log",
        "//core/common:core",
    ]
)

cc_library(
    name = "mock_timestamp_controller",
    srcs = ["mock_timestamp_driver.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/timestamp:timestamp_controller",
    ],
    testonly = True,
)

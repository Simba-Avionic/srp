cc_test(
    name = "core_test",
    srcs = [
        "common.cc",
        "time.cc",
        "json.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/common:common_converter",
        "//core/common:common_types",
        "@com_google_googletest//:gtest_main",
        "//core/time:sys_time_change_lib",
        "//core/json:simba_json",
    ],
)

cc_test(
    name = "crc_16",
    srcs = ["test_crc_16.cc"],
    deps = [
        "//core/common:crc_lib",
        "@com_google_googletest//:gtest_main",
    ],
    size = "medium",
)

cc_test(
    name = "wait_queue_test",
    srcs = ["wait_queue.cc"],
    deps = [
        "//core/common:wait_queue",
        "@com_google_googletest//:gtest_main",
    ],
    size = "medium",
)
cc_test(
    name = "condition_test",
    srcs = ["condition_test.cc"],
    deps = [
        "//core/common:condition_lib",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "csv_driver_test",
    srcs = ["csv_driver_test.cc"],
    deps = [
        "//core/csvdriver",
        "@com_google_googletest//:gtest_main",
    ],
)
cc_test(
    name = "gpio_driver_test",
    srcs = ["test_gpio_driver.cpp"],
    deps = [
        "//core/gpio:gpio_drivers",
        "//core/file:mock_file",
        "@com_google_googletest//:gtest_main",
    ],
)
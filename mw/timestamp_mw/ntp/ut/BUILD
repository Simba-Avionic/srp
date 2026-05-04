cc_test(
    name = "controller_test",
    srcs = [
        "controler_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/timestamp_mw/ntp/controller:ntp_controller",
    ],
)

cc_test(
    name = "discovery_manager_test",
    size = "small",
    srcs = ["discovery_manager_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/timestamp_mw/ntp/discovery:ntp_discovery",
    ],
)

cc_test(
    name = "config_manager_test",
    size = "small",
    srcs = ["config_manager_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/timestamp_mw/ntp/config:ntp_config_manager",
    ],
)
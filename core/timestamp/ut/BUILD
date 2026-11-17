cc_test(
    name = "timestamp_test",
    srcs = [
        "timestamp_driver_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/timestamp:timestamp_controller",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "timestamp_audit_bugs_test",
    srcs = ["test_audit_bugs.cc"],
    tags = ["audit_bugs"],
    deps = [
        "//core/timestamp:timestamp_controller",
        "@com_google_googletest//:gtest_main",
    ],
)

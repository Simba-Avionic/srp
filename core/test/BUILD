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

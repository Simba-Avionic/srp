cc_test(
    name = "config_controller_test",
    srcs = ["config_controller_test.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/config_controller:diag_config_controller",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "config_controller_json",
    srcs = ["config_controller_json_test.cc"],
    data = ["//diag/config_controller/test/resource"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/config_controller:diag_config_parser",
        "@com_google_googletest//:gtest_main",
        "@com_json//:json",
    ],
)

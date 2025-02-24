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
    name = "header_test",
    srcs = [
        "header_test.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/timestamp_mw/ntp/header:tinyNtp_header",
    ],
)

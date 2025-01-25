cc_test(
    name = "gps_app_test",
    srcs = [
        "gps_app_test.cc"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//apps/fc/gps_service:gps_app_lib",
        "//core/uart:mock_uart",
    ],
)
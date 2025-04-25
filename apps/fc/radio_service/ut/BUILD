cc_test(
    name = "radio_app_test",
    srcs = [
        "radio_app_test.cc"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//apps/fc/radio_service:radio_app_lib",
        "//core/uart:mock_uart",
        "//core/timestamp:mock_timestamp_controller",
    ],
)
cc_test(
    name = "uart_test",
    srcs = ["uart_test.cc"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/uart:uart_driver",
    ],
    size = "medium",
)


cc_test(
    name = "uart_config_test",
    srcs = ["uart_config_test.cc"],
    linkopts = ["-lutil"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/uart:mock_uart",
        "//core/uart:uart_driver",
    ],
)

cc_test(
    name = "uart_wait_test",
    srcs = ["uart_wait_test.cc"],
    linkopts = ["-lutil"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/uart:uart_driver",
        "//core/sim:sim_ctl",
    ],
)

cc_test(
    name = "uart_audit_bugs_test",
    srcs = ["test_audit_bugs.cc"],
    tags = ["audit_bugs"],
    copts = ["-fsanitize=address", "-fno-omit-frame-pointer"],
    linkopts = ["-fsanitize=address", "-lutil"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/uart:uart_driver",
    ],
)

cc_test(
    name = "uart_regression_test",
    srcs = ["test_regression.cc"],
    copts = ["-fsanitize=address", "-fno-omit-frame-pointer"],
    linkopts = ["-fsanitize=address", "-lutil"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//core/uart:uart_driver",
    ],
)

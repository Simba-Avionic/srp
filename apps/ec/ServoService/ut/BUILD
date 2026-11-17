cc_test(
    name = "servo_audit_bugs_test",
    srcs = ["test_audit_bugs.cpp"],
    tags = ["audit_bugs"],
    data = ["//deployment/apps/servo_service:config.json"],
    deps = [
        "//apps/ec/ServoService/servoController:servo_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "xo_correlation_test",
    srcs = ["test_xo_correlation.cpp"],
    deps = [
        "//apps/ec/ServoService/servoController:servo_controller",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "servo_config_channel_test",
    srcs = ["test_config_channel.cpp"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//apps/ec/ServoService/servoController:servo_controller",
    ],
)

cc_test(
    name = "servo_errors_test",
    srcs = ["test_servo_errors.cpp"],
    deps = [
        "//apps/ec/ServoService/servoController:servo_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "servo_state_test",
    srcs = ["servo_state_test.cc"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//apps/ec/ServoService/servoController:servo_controller",
    ],
)

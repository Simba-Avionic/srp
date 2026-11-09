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

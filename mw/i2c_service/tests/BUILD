cc_test(
    name = "data_structure",
    srcs = [
        "test_header.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/data:i2c_factory"
    ],
)


# cc_test(
#     name = "pca9685_test",
#     srcs = [
#         "test_pca9685.cpp"
#         ],
#     visibility = ["//visibility:public"],
#     deps=[
#         "@com_google_googletest//:gtest_main",
#         "//mw/i2c_service/controller/pca9685:pca9685_controller",
#         "//mw/gpio_server/controller/mock:mock_gpio_controller",
#         "//mw/i2c_service/controller/mock:mock_i2c_controller",
#         "@bazel_tools//tools/cpp/runfiles",
#     ],
#     data = [
#         "//mw/i2c_service/tests/pca_files:pca9685_test_data"
#     ],

# )
cc_test(
    name = "ads7828_test",
    srcs = [
        "test_ads7828.cc"
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/ads7828:adc_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
)
cc_test(
    name = "adcsensor_test",
    srcs = [
        "test_adcsensor.cc"
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/adcsensor:adc_sensor_controller",
        "//mw/i2c_service/controller/mock:mock_ads7828",
        "//core/i2c/mock:mock_i2c",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
    data = [
        "//mw/i2c_service/tests/test_adcsensor:adc_test_data",
    ],
)
cc_test(
    name = "i2c_controller_test",
    srcs = [
        "test_i2ccontroller.cc",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//communication-core/sockets/mock:mock_socket",
        "//mw/i2c_service/controller:i2c_controller",
    ],
)

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

cc_test(
    name = "pca9685_audit_bugs_test",
    srcs = ["test_audit_bugs.cpp"],
    tags = ["audit_bugs"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
)

cc_test(
    name = "pca9685_channel_test",
    srcs = [
        "test_pca9685_channel.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
)

cc_test(
    name = "pca9685_short_read_test",
    srcs = [
        "test_pca9685_short_read.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
)

cc_test(
    name = "adcsensor_calibration_test",
    srcs = ["test_adcsensor_calibration.cc"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/adcsensor:adc_sensor_controller",
        "//mw/i2c_service/controller/mock:mock_ads7828",
    ],
)

cc_test(
    name = "eeprom_writebyte_test",
    srcs = ["test_eeprom_writebyte.cc"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/24lc32at:controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
)

cc_test(
    name = "ina219_init_test",
    srcs = [
        "test_ina219_init.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/ina219:ina219_controller",
        "//communication-core/sockets/mock:mock_socket",
    ],
)

cc_test(
    name = "pca9685_block_write_test",
    srcs = [
        "test_pca9685_block_write.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
)

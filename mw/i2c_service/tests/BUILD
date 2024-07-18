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
cc_test(
    name = "pca9685_test",
    srcs = [
        "test_pca9685.cpp"
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/pca9685:pca9685_controller",
        "//mw/gpio_server/controller/mock:mock_gpio_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
    ],
    disabled = True,
)
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
    ],
)

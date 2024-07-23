cc_library(
    name = "mock_i2c_controller",
    srcs = [
        "mock_i2ccontroller.h",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller:i2c_controller",
    ],
    testonly = True,
)
cc_library(
    name = "mock_ads7828",
    srcs = [
        "mock_ads7828.h",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/ads7828:adc_controller",
    ],
    testonly = True,
)

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
        "test_controller.cpp"
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/controller/pca9685:pca9685_controller"
    ],
)

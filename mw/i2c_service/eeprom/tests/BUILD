cc_test(
    name = "data_structure",
    srcs = [
        "test_header.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/eeprom/data:eeprom_hdr",
    ]
)
cc_test(
    name = "data_factory",
    srcs = [
        "test_factory.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/i2c_service/eeprom/factory:eeprom_factory",
    ]
)
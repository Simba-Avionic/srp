

cc_library(
    name = "primer_controller",
    deps = [
        "//mw/gpio_server/controller:gpio_controller",
        "//core/common:condition_lib",
        "//core/json:simba_json",
        "//mw/i2c_service/controller/ads7828:adc_controller",
    ],
    srcs = [
        "primer_controller.cpp",
    ],
    hdrs = [
        "primer_controller.hpp",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "primer_controller_test",
    srcs = ["primer_controller_test.cpp"],
    deps = [
        ":primer_controller",
        "//mw/i2c_service/controller/mock:mock_i2c_controller",
        "@com_google_googletest//:gtest_main",
    ],
)

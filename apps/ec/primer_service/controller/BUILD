

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

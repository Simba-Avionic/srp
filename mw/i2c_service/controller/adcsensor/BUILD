cc_library(
    name = "adc_sensor_controller",
    deps = [
        "//mw/i2c_service/controller/ads7828:adc_controller",
        "//core/json:simba_json",
    ],
    srcs = ["controller.cpp"],
    hdrs = ["controller.hpp"],
    visibility = [
        "//apps:__subpackages__",
        "//mw/i2c_service/tests:__subpackages__",
        ],
)

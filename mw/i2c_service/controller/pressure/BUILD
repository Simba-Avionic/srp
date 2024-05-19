cc_library(
    name = "pressure_controller",
    deps = [
        "//core/logger:Logger",
        "//mw/i2c_service/controller/ads7828:adc_controller",
        "//core/json:simba_json",
    ],
    srcs = ["controller.cpp"],
    hdrs = ["controller.hpp"],
    visibility = [
        "//apps:__subpackages__",
        ],
)
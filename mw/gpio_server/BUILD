cc_binary(
    name = "gpio_service",
    srcs = [
        "gpio_mw.cpp",
        "gpio_mw.hpp"
        ],
    deps = [
        "//core/gpio:gpio_drivers",
        "//mw/gpio_server/data:gpio_hdr",
        "//core/application:simba_application",
        "//core/json:simba_json",
    ],
)
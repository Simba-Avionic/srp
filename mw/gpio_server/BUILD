cc_library(
    name = "gpio_service_lib",
    srcs = [
        "gpio_mw.cpp",
        ],
    hdrs=[
        "gpio_mw.hpp"
    ],
    deps = [
        "//core/gpio:gpio_drivers",
        "//mw/gpio_server/data:gpio_hdr",
        "//core/application:simba_application",
        "//core/json:simba_json",
    ],
    visibility = [
        "//mw/gpio_server/tests:__subpackages__",
        ],
)

cc_binary(
    name = "gpio_service",
    deps = [":gpio_service_lib"],
    srcs = ["main.cpp"],
    visibility = [
        "//deployment:__subpackages__",
    ],
)
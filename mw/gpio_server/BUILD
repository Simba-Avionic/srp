gpio_drivers_deps = select({
    "//:sim": ["//core/gpio:sim_gpio_drivers"],
    "//conditions:default": ["//core/gpio:gpio_drivers"],
})

cc_library(
    name = "gpio_service_lib",
    srcs = [
        "gpio_mw.cpp",
        ],
    hdrs=[
        "gpio_mw.hpp",
        "gpio_mw_did.h",
    ],
    deps = [
        "//mw/gpio_server/data:gpio_hdr",
        "//core/json:simba_json",
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//communication-core/sockets:socket_ipc",
        "@srp_platform//ara/diag:uds_lib",
    ] + gpio_drivers_deps,
    visibility = [
        "//mw/gpio_server/tests:__subpackages__",
        ],
)

cc_binary(
    name = "gpio_service",
    deps = [
        ":gpio_service_lib",
        "//deployment/mw/gpio:ara",
        ],
    srcs = [
        "main.cpp",
        ],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

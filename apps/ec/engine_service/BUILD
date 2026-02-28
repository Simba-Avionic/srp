
cc_binary(
    name = "engine_service",
    srcs = [
        "main.cpp",
        "engine_app.cpp",
        "engine_app.hpp",
        "service.hpp",
        "service.cpp",
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
        ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/engine_app:ara",
        "//deployment/apps/engine_app:someip_lib",
        "//mw/gpio_server/controller:gpio_controller",
        "//core/rocket_machine_state:rocket_state_base",
    ],
)
cc_binary(
    name = "blinking_service",
    deps = [
        "//deployment/apps/blinking_service:ara",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//mw/gpio_server/controller:gpio_controller",
    ],
    srcs = ["blinking_service.cpp", "main.cpp", "blinking_service.hpp"],
    visibility = ["//deployment/apps/blinking_service:__subpackages__"],
)

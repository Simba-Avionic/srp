cc_binary(
    name = "test_app",
    deps = [
        "//deployment/apps/test_app:ara",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//mw/gpio_server/controller:gpio_controller",
    ],
    srcs = ["main.cpp", "test.cpp", "test.hpp"],
    visibility = ["//deployment/apps/test_app:__subpackages__"],
)
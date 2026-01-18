cc_binary(
    name = "gh_service",
    srcs = [
        "main.cpp",
        "gh_app.cpp",
        "gh_app.hpp",
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
    ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//deployment/apps/gh_service:ara",
        "//core/common:condition_lib",
        "//mw/gpio_server/controller:gpio_controller",
        "//mw/i2c_service/controller/bme280:bme280_controller",
    ]

)
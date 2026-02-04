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
        "//mw/i2c_service/controller/ads7828:adc_controller",
        "//deployment/apps/gh_service:someip_lib"
    ]

)
cc_binary(
    name = "fuel_service",
    srcs = [
        "main.cpp",
        "fuel_app.cpp",
        "fuel_app.hpp",
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
        ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/fuel_service:ara",
    ],
)
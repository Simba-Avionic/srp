load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")
ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/fc/apogee_detect_service:instance"],
    visibility = ["//apps/fc/apogee_service:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/fc/apogee_detect_service:instance"],
    visibility = ["//apps/fc/apogee_service:__subpackages__"],
)

cc_binary(
    name = "apogee_service_fc",
    srcs = [
        "main.cc",
        "apogee_app.cpp",
        "apogee_app.hpp",
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
    ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",

        ":ara",
        ":someip_lib",

        "//core/apogee:apogee_lib", 
    ],
)

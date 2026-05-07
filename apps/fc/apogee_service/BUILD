load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")
//skopiowane z env_service, nie wiem czy dobrze
ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/apps/fc/apogee_service:instance"],
    visibility = ["//apps/fc/apogee_service:__subpackages__"],
)

ara_someip_lib(
    name = "someip_lib",
    model_src = ["//deployment/apps/fc/apogee_service:instance"],
    visibility = ["//apps/fc/apogee_service:__subpackages__"],
)

cc_binary(
    name = "apogee_service_fc",
    srcs = [
        "main.cpp",
        "apogee_service.cpp",
        "apogee_service.hpp",
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
    ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",

        "//deployment/apps/fc/apogee_service:ara",
        "//deployment/apps/fc/apogee_service:someip_lib",

        "//deployment/apps/fc/env_service:someip_lib",
        "//deployment/apps/fc/nav_service:someip_lib", 
    
        "//core/apogee:apogee_detector_lib", 
    ],
)
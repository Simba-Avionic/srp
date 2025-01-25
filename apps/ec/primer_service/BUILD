cc_binary(
    name = "primer",
    srcs = [
        "main.cc",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/prim_service:ara",

        "//apps/ec/primer_service",
        "//apps/ec/primer_service:primer_app",
    ],
)

cc_library(
    name = "primer_service",
    deps = [
        "//apps/ec/primer_service/controller:primer_controller",
        "//deployment/apps/prim_service:someip_lib",
    ],
    srcs = [
    ],
    hdrs = [
        "service.hpp",
    ],
)
cc_library(
    name = "primer_app",
    deps = [
        "//apps/ec/primer_service",
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
    ],
    srcs = [
        "app.cpp",
    ],
    hdrs = [
        "app.hpp",
    ],
)
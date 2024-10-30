cc_binary(
    name = "primer",
    srcs = [
        "main.cc",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/prim_service:ara",

        "//apps/primer_service",
        "//apps/primer_service:primer_app",
    ],
)

cc_library(
    name = "primer_service",
    deps = [
        "//apps/primer_service/controller:primer_controller",
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
        "//apps/primer_service",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
    ],
    srcs = [
        "app.cpp",
    ],
    hdrs = [
        "app.hpp",
    ],
)
cc_binary(
    name = "primer",
    srcs = [
        "primer_service.cpp",
        "primer_service.hpp",
        "main.cc",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "//mw/gpio_server/controller:gpio_controller",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/prim_service:someip_lib",
        "//deployment/apps/prim_service:ara",
        "//core/json:simba_json",
    ],
)

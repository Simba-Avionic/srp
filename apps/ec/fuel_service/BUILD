cc_library(
    name = "fuel_config_reader",
    hdrs = ["fuel_config_reader.hpp"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/json:simba_json",
        "//apps/ec/fuel_service/controller:refuel_controller",
        "@srp_platform//ara/log",
    ],
)

cc_binary(
    name = "fuel_service",
    srcs = [
        "main.cpp",
        "fuel_app.cpp",
        "fuel_app.hpp",
        "service.cpp",
        "service.hpp"
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
        ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/fuel_service:ara",
        "//deployment/apps/fuel_service:someip_lib",
        "//apps/ec/fuel_service/controller:refuel_controller",
        "//core/json:simba_json",
        ":fuel_config_reader",
    ],
)
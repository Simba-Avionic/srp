cc_binary(
    name = "logger_service",
    srcs = [
        "logger_service.cpp",
        "logger_service.hpp",
        "main.cc",
        "service.hpp",
        "logger_builder.hpp",
        "logger_did.hpp",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//core/json:simba_json",
        "//deployment/apps/logger_service:someip_lib",
        "//deployment/apps/logger_service:ara",
        "//core/timestamp:timestamp_controller",
        "//core/csvdriver",
        "//apps/ec/logger_service:logger_data_type_lib",
        "//core/time:sys_time_change_lib",
        "@srp_platform//ara/diag:uds_lib",
    ],
)

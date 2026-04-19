cc_binary(
    name = "logger_service_fc",
    srcs = [
        "logger_service.cpp",
        "logger_service.hpp",
        "main.cpp",
        "service.hpp",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//apps/fc/logger_service:logger_data_type_lib",
        "//core/common:condition_lib",
        "//core/csvdriver",
        "//core/time:sys_time_change_lib",
        "//core/timestamp:timestamp_controller",
        "//deployment/apps/fc/logger_service:ara",
        "//deployment/apps/fc/logger_service:someip_lib",
    ],
)

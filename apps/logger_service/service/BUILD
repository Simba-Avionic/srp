cc_binary(
    name = "logger_service",
    srcs = [
        "logger_service.cpp",
        "logger_service.hpp",
        "main.cc",
        "service.hpp",
        "logger_did.hpp",
        "logger_builder.hpp",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//diag/jobs/skeleton:did_job_lib",
        "//core/json:simba_json",
        "//deployment/apps/logger_service:someip_lib",
        "//deployment/apps/logger_service:ara",
        "//core/timestamp:timestamp_controller",
        "//core/csvdriver",
        "//apps/logger_service:logger_data_type_lib",
        "//core/time:sys_time_change_lib",
    ],
)

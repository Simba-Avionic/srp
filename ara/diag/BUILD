cc_library(
    name = "monitor_lib",
    srcs = ["monitor.cc"],
    hdrs = [
        "monitor.h",
        "monitor_types.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/core:Result",
        "//ara/core:instance_specifier",
    ],
)

cc_library(
    name = "diag_error_domain",
    srcs = [
        "diag_error_domain.cc",
        "uds_error_domain.cc",
    ],
    hdrs = [
        "diag_error_domain.h",
        "uds_error_domain.h",
    ],
    visibility = ["//ara/diag:__subpackages__"],
    deps = ["//ara/core:error_lib"],
)

cc_library(
    name = "uds_lib",
    srcs = [
        "generic_data_identifier.cc",
        "generic_routine.cc",
    ],
    hdrs = [
        "generic_data_identifier.h",
        "generic_routine.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/diag:diag_error_domain",
        "//ara/diag/routing",
    ],
)

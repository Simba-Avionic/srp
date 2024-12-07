load("//deployment/tools/ara:data_structure_generator.bzl", "data_structure_generator")

data_structure_generator(
    name = "dtc_com_data",
    model = ["//deployment/system_definition/platform/data_structure:dtc_com_data_structure"],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "monitor",
    srcs = ["monitor.cc"],
    hdrs = [
        "monitor.h",
        "monitor_types.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/core:Result",
        "//ara/core:instance_specifier",
        "//ara/core:model_db_lib",
        "//ara/diag:diagnostic_menager",
        "//core/common:condition_lib",
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

cc_library(
    name = "diagnostic_menager",
    srcs = ["diagnostic_menager.cc"],
    hdrs = ["diagnostic_menager.h"],
    visibility = ["//ara:__subpackages__"],
    deps = [
        "//ara/com:com_controller",
        "//ara/diag:diag_error_domain",
        "//ara/diag:dtc_com_data",
    ],
)

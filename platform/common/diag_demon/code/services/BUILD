cc_library(
    name = "uds_server",
    srcs = [
        "uds_server.cc",
        "validator_controller.cc",
    ],
    hdrs = [
        "uds_server.h",
        "validator_controller.h",
    ],
    visibility = ["//platform/common/diag_demon/code:__subpackages__"],
    deps = [
        "//ara/log",
        "//libdoip/libdoipserver",
        "//platform/common/diag_demon/code/services:uds_data_types",
        "//platform/common/diag_demon/code/services:uds_interfaces",
    ],
)

cc_library(
    name = "example_did",
    hdrs = ["example_did.h"],
    visibility = ["//platform/common/diag_demon/code:__subpackages__"],
    deps = [
        "//ara/log",
        "//ara/diag:uds_lib",
    ],
)

cc_library(
    name = "uds_data_types",
    hdrs = [
        "uds_nrc_codes.h",
        "uds_request.h",
    ],
    visibility = ["//platform/common/diag_demon/code:__subpackages__"],
)

cc_library(
    name = "uds_interfaces",
    hdrs = ["i_validator.h"],
    visibility = ["//platform/common/diag_demon/code:__subpackages__"],
    deps = [":uds_data_types"],
)

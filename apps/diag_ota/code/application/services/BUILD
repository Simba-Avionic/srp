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
    visibility = ["//apps/diag_ota/code/application:__subpackages__"],
    deps = [
        "//apps/diag_ota/code/application/services:uds_data_types",
        "//apps/diag_ota/code/application/services:uds_interfaces",
        "//core/logger:Logger",
        "//libdoip/libdoipserver",
    ],
)

cc_library(
    name = "uds_data_types",
    hdrs = [
        "uds_nrc_codes.h",
        "uds_request.h",
    ],
    visibility = ["//apps/diag_ota/code/application/services:__subpackages__"],
)

cc_library(
    name = "uds_interfaces",
    hdrs = ["i_validator.h"],
    visibility = ["//apps/diag_ota/code/application/services:__subpackages__"],
    deps = [":uds_data_types"],
)

load("//deployment/tools/ara:data_structure_generator.bzl", "data_structure_generator")

data_structure_generator(
    name = "dtc_com_data",
    model = ["//deployment/system_definition/platform/data_structure:dtc_com_data_structure"],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "dtc",
    srcs = ["dtc_service.cc"],
    hdrs = ["dtc_service.h"],
    visibility = ["//platform/common/diag_demon/code:__subpackages__"],
    deps = [
        ":dtc_interface",
        "//ara/com/socket:ipc_socket",
        "//ara/log",
        ":dtc_com_data"
    ],
)

cc_library(
    name = "dtc_interface",
    hdrs = ["i_dtc_service.h"],
    visibility = ["//platform/common/diag_demon/code:__subpackages__"],
    deps = [],
)

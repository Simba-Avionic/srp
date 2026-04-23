load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("@srp_platform//tools/model_generator/ara:data_structure_generator.bzl", "data_structure_generator")

cc_library(
    name = "heartbeat_controller",
    visibility = ["//visibility:public"],
    deps = [
        ":hb_com_data",
        "//core/common:core",
        "//communication-core/sockets:socket_ipc",
        "@srp_platform//ara/log",
    ],
    hdrs = [
        "controller.hpp",
        "common.hpp",
    ],
    srcs = [
        "controller.cpp",
    ]
)

cc_library(
    name = "heartbeat_master",
    visibility = ["//visibility:public"],
    deps = [
        ":hb_com_data",
        "//core/common:core",
        "//communication-core/sockets:socket_ipc",
        "@srp_platform//ara/log",
    ],
    hdrs = [
        "master.hpp",
        "common.hpp",
    ],
    srcs = [
        "master.cpp",
    ]
)

data_structure_generator(
    name = "hb_com_data",
    model = [
        "//deployment/system_definition/platform/data_structure:hb_data_structure",
    ],
    visibility = ["//visibility:private"],
)
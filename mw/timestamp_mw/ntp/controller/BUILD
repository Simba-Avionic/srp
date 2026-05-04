load("@srp_platform//tools/model_generator/ara:data_structure_generator.bzl", "data_structure_generator")
cc_library(
    name = "ntp_controller",
    deps = [ 
        "//communication-core/sockets:socket_tcp",
        "//core/json:simba_json",
        "//core/timestamp:timestamp_controller",
        "//mw/timestamp_mw/ntp/controller:ntp_com_data",
        "//mw/timestamp_mw/ntp/config:ntp_config_manager",
        "//mw/timestamp_mw/ntp/discovery:ntp_discovery",
    ],
    srcs = ["ntp_controller.cpp"],
    hdrs = ["ntp_controller.hpp"],
    visibility = ["//visibility:public"],
)

data_structure_generator(
    name = "ntp_com_data",
    model = [
        "//deployment/system_definition/platform/data_structure:tinyNTP_com_data_structure",
    ],
    visibility = ["//visibility:private"],
)
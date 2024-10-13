cc_library(
    name = "connector",
    srcs = ["service_connector.cc"],
    hdrs = ["service_connector.h"],
    visibility = ["//platform/common/someip_demon/code:__subpackages__"],
    deps = [
        "//ara/com/someip:someip_data",
        "//ara/log",
        "//core/common:condition_lib",
        "//platform/common/someip_demon/code/sd:sd_controller",
        "//platform/common/someip_demon/code/common:udp_lib",
        "//communication-core/sockets:socket_ipc",
    ],
)

cc_library(
    name = "dlt_service_lib",
    srcs = ["dlt_service.cc"],
    hdrs = ["dlt_service.h"],
    visibility = ["//platform/common/logger/code:__subpackages__"],
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//communication-core/sockets:socket_udp_multicast",
        "//ara/exec:adaptive_application_lib",
        "//core/common:wait_queue",
        "//core/json:simba_json",
        "//platform/common/logger/code/data:dlt_data_types",
    ],
)

cc_library(
    name = "dlt_service_lib",
    srcs = ["dlt_service.cc"],
    hdrs = ["dlt_service.h"],
    visibility = ["//mw/logger:__subpackages__"],
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//communication-core/sockets:socket_udp_multicast",
        "//core/application:simba_application",
        "//core/common:wait_queue",
        "//core/json:simba_json",
        "//mw/logger/code/data:dlt_data_types",
    ],
)

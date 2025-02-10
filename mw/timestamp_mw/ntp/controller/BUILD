cc_library(
    name = "ntp_controller",
    deps = [
        "//mw/timestamp_mw/ntp/header:tinyNtp_header",
        "//communication-core/sockets:socket_tcp",
        "//core/json:simba_json",
        "//core/timestamp:timestamp_controller",
    ],
    srcs = ["ntp_controller.cpp"],
    hdrs = ["ntp_controller.hpp"],
    visibility = ["//visibility:public"],
)
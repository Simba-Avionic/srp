cc_library(
    name = "interfaces",
    hdrs = [
        "callbacks.h",
        "iproxy.h",
        "iskeleton.h",
    ],
    visibility = [],
    deps = [
        "//communication-core/someip:someip_types",
        "//communication-core/someip-database/code:someip_database_lib",
    ],
)

cc_library(
    name = "proxy",
    srcs = [],
    hdrs = [
        "event_proxy.h",
        "method_proxy.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interfaces",
        "//communication-core/someip-database/code:app_config_object",
        "//core/logger:Logger",
    ],
)

cc_library(
    name = "skeleton",
    srcs = [],
    hdrs = [
        "event_skeleton.h",
        "method_skeleton.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interfaces",
        "//communication-core/someip-database/code:app_config_object",
        "//core/logger:Logger",
    ],
)

cc_library(
    name = "transfer_lib",
    hdrs = ["transfer.h"],
    visibility = [
        "//core:__subpackages__",
        "//diag:__subpackages__",
    ],
)

cc_library(
    name = "controller",
    srcs = ["controller.cc"],
    hdrs = ["controller.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//communication-core/sockets:socket_udp",
        "//communication-core/someip:someip_objects",
        "//communication-core/someip-controller:interfaces",
        "//communication-core/someip-database/code:someip_database_lib",
        "//core/common:wait_queue",
        "//core/logger:Logger",
    ],
)

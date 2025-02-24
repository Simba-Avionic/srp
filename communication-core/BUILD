load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "communication_core",
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/network-data:interfaces",
        "//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type",
        "//communication-core/sockets:socket_ipc",
        "//communication-core/sockets:socket_udp",
    ],
)
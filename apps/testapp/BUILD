cc_binary(
    name = "app1",
    srcs = [
        "app1.cc",
    ],
    visibility = ["//deployment/apps/diag_ota:__subpackages__"],
    deps = [
        "//communication-core/sockets:socket_ipc"
    ],
)

cc_binary(
    name = "app2",
    srcs = [
        "app2.cc",
    ],
    visibility = ["//deployment/apps/diag_ota:__subpackages__"],
    deps = [
        "//communication-core/sockets:socket_ipc"
    ],
)
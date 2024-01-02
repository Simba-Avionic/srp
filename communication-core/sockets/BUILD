cc_library(
    name = "socket_interface",
    hdrs = [
        "Isocket.h",
        "socket_config.h",
    ],
    deps = ["//core"],
    visibility = ["//communication-core:__subpackages__"],
)

cc_library(
    name = "socket_ipc",
    srcs = [
        "ipc_socket.cc",
    ],
    hdrs = [
        "ipc_socket.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [":socket_interface"],
)

cc_library(
    name = "socket_udp",
    srcs = [
        "udp_socket.cc",
    ],
    hdrs = [
        "udp_socket.h",
    ],
    visibility = ["//communication-core:__subpackages__"],
    deps = [":socket_interface","//core"],
)

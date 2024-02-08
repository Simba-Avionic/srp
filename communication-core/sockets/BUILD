cc_library(
    name = "socket_interface",
    hdrs = [
        "Isocket.h",
        "socket_config.h",
    ],
    visibility = [
        "//communication-core:__subpackages__",
        "//diag:__subpackages__",
        "//mw:__subpackages__",
    ],
    deps = ["//core/common:common_types"],
)

cc_library(
    name = "socket_ipc",
    srcs = [
        "ipc_socket.cc",
    ],
    hdrs = [
        "ipc_socket.h",
    ],
    visibility = [
        "//apps:__subpackages__",
        "//communication-core:__subpackages__",
        "//core:__subpackages__",
        "//diag:__subpackages__",
        "//mw:__subpackages__",
    ],
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
    visibility = [
        "//communication-core:__subpackages__",
        "//diag/base/controller:__subpackages__",
        "//mw:__subpackages__",
    ],
    deps = [
        ":socket_interface",
        "//core/common:common_types",
    ],
)

cc_library(
    name = "socket_udp_multicast",
    srcs = [
        "udp_multicast_socket.cc",
    ],
    hdrs = [
        "udp_multicast_socket.h",
    ],
    visibility = [
        "//mw:__subpackages__",
    ],
    deps = [
        ":socket_interface",
        "//core/common:common_types",
    ],
)

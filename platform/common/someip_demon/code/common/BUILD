cc_library(
    name = "udp_lib",
    srcs = [
        "udp_multicast_socket.cc",
        "udp_socket.cc",
    ],
    hdrs = [
        "udp_multicast_socket.h",
        "udp_socket.h",
    ],
    data = [],
    visibility = [
        "//platform/common/logger/code/application:__subpackages__",
        "//platform/common/someip_demon/code:__subpackages__",
    ],
    deps = [
        "//ara/log",
        "//core/common:common_types",
    ],
)

cc_library(
    name = "controller",
    srcs = ["network_controller.cc"],
    hdrs = [
        "i_controller.h",
        "i_network_controller.h",
        "network_controller.h",
    ],
    visibility = ["//platform/common/someip_demon/code:__subpackages__"],
    deps = [
        "//ara/com/someip:someip_data",
        "//ara/core:Result",
        "//ara/log",
    ],
)

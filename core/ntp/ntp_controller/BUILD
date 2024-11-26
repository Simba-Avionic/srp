cc_library(
    name = "ntp_controller",
    deps = [
        "//ara/com/shm:shm_lib",
        "//ara/log",
        "//communication-core/sockets:socket_udp",
        "//core/json:simba_json",
        "//core/common:core",
    ],
    hdrs = [
        "ntp_controller.hpp",
    ],
    srcs = [
        "ntp_controller.cpp",
    ],
    visibility = [
        "//example_adaptive/ExampleApp/code/application:__subpackages__",
        "//platform/common/em:__subpackages__",
        ],
)
cc_library(
    name = "diag_transfer",
    hdrs = ["diag_transfer.h"],
    visibility = [],
    deps = [
        "//communication-core/someip-controller:transfer_lib",
        "//core/common:common_types",
    ],
)

cc_library(
    name = "idiag_controller",
    hdrs = ["idiag_controller.h"],
    deps = [
        "//core/common:common_types",
    ],
)

cc_library(
    name = "diag_controller",
    srcs = ["diag_controller.cc"],
    hdrs = ["diag_controller.h"],
    visibility = [
        "//apps:__subpackages__",
        "//core:__subpackages__",
        "//mw:__subpackages__",
    ],
    deps = [
        "//communication-core/sockets:socket_interface",
        "//core/common:common_types",
        "//core/logger:Logger",
        "//diag/base/controller:diag_transfer",
        "//diag/base/controller:idiag_controller",
        "//diag/base/data:diag_parser",
        "//diag/base/factories:diag_data_factory",
    ],
)

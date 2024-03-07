cc_library(
    name = "ota_lib",
    srcs = ["ota_service.cc"],
    hdrs = ["ota_service.h"],
    visibility = ["//mw/diag/ota:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "//diag/base/controller:diag_controller",
        "//communication-core/sockets:socket_ipc",
    ],
)

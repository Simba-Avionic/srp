cc_library(
    name = "diag-app",
    srcs = ["diag-ota.cc"],
    hdrs = ["diag-ota.h"],
    visibility = ["//apps/diag_ota:__subpackages__"],
    deps = [
        "//apps/diag_ota/code/application/services:uds_server",
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//core/application:simba_application",
        "//core/json:simba_json",
        "//diag/base/controller:diag_controller",
    ],
)

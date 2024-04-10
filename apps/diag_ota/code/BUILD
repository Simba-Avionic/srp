cc_binary(
    name = "diag_ota",
    srcs = [
        "main.cc",
    ],
    visibility = ["//deployment/apps/diag_ota:__subpackages__"],
    deps = [
        "//apps/diag_ota/code/application:diag-app",
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//core/application:simba_application",
    ],
)
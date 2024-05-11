cc_binary(
    name = "jaku_service",
    srcs = [
        "jaku_service.cc",
        "jaku_service.h",
        "main.cc"
    ],
    visibility = [
        "//deployment/apps:__subpackages__",
    ],
    deps = [
        "//mw/temp/controller:temp_controller_mw",
        "//communication-core/someip-controller:skeleton",
        # "//communication-core/someip-controller:",
        "//core/application:simba_application",
        "@untar",
    ]
)
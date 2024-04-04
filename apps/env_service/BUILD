cc_binary(
    name = "env_service",
    srcs = [
        "main.cpp",
        "env_service.cpp",
        "env_service.hpp"
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
        ],
    deps = [
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//core/application:simba_application",
        "//mw/temp/controller:temp_controller_mw",
        "@untar",
    ],
)
cc_binary(
    name = "ServoService",
    srcs = [
        "main.cc",
        "servoService.cpp",
        "servoService.hpp",
    ],
    visibility = [
        "//deployment/apps/servo_service:__subpackages__"
        ],
    deps = [
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//core/application:simba_application",
        "@untar",
    ],
)

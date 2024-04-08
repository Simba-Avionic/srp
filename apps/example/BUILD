cc_binary(
    name = "example",
    srcs = [
        "main.cc",
        "router.cc",
        "router.h",
    ],
    visibility = ["//deployment/apps/example:__subpackages__"],
    deps = [
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//core/application:simba_application",
        "//diag/dtc/controller:diag_dtc_",
        "//mw/gpio_server/controller:gpio_controller",
        "//mw/i2c_service/pca9685/controller:servoController",
        "@untar",
    ],
)

cc_binary(
    name = "example2",
    srcs = [
        "main.cc",
        "router.h",
        "router2.cc",
    ],
    visibility = ["//deployment/apps/example2:__subpackages__"],
    deps = [
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//core/application:simba_application",
        "//mw/gpio_server/controller:gpio_controller",
        "@untar",
    ],
)

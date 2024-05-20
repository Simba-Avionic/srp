cc_binary(
    name = "homework",
    srcs = [
        "main.cc",
        "homework_app.cc",
        "homework_app.h",
    ],
    visibility = ["//deployment/apps/homework:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "//mw/gpio_server/controller:gpio_controller",
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//diag/dtc/controller:diag_dtc_",

    ],
)

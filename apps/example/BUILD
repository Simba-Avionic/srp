cc_binary(
    name = "example",
    srcs = [
        "main.cc",
        "router.cc",
        "router.h",
    ],
    visibility = ["//deployment/apps/example:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "@untar",
        "//mw/gpio_server/controller:gpio_controller",
        "//mw/i2c_service/pca9685/controller:servoController"
    ],
)

# cc_binary(
#     name = "router2",
#     srcs = [
#         "main.cc",
#         "router.h",
#         "router2.cc",
#     ],
#     visibility = ["//deployment/apps/example:__subpackages__"],
#     deps = [
#         "//communication-core/sockets:socket_ipc",
#         "//core",
#         "//diag/base/controller:diag_controller",
#         "@untar",
#     ],
# )

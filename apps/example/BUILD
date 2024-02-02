cc_binary(
    name = "router",
    srcs = [
        "main.cc",
        "router.cc",
        "router.h",
    ],
    visibility = ["//deployment/apps/example:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "@untar",
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

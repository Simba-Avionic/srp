cc_library(
    name = "bindings_lib",
    srcs = [
        "binding_factory.cc",
        "ipc_bindings.cc",
        "proxy/udp_proxy_bindings.cc",
        "skeleton/udp_skeleton_bindings.cc",
    ],
    hdrs = [
        "binding_factory.h",
        "common_bindings.h",
        "ipc_bindings.h",
        "proxy/udp_proxy_bindings.h",
        "skeleton/udp_skeleton_bindings.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com:com_error_domain",
        "//ara/com:com_log",
        "//ara/com/shm:shm_buffor_lib",
        "//ara/com/someip:someip_data",
        "//ara/com/someip/bindings:binding_interface",
        "//ara/com/someip/controller",
        "//ara/core:condition_variable",
        "//ara/core:string_view",
        "//ara/core/model:Model_lib",
        "//communication-core/sockets:socket_ipc",
        "//core/common:condition_lib",
    ],
)

cc_library(
    name = "binding_interface",
    hdrs = ["i_bindings.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com/someip:someip_data",
        "//ara/core:Result",
        "//ara/core/model:Model_lib",
    ],
)

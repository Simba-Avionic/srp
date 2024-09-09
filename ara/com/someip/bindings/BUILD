cc_library(
    name = "bindings_lib",
    srcs = [
        "binding_factory.cc",
        "ipc_bindings.cc",
    ],
    hdrs = [
        "binding_factory.h",
        "common_bindings.h",
        "i_bindings.h",
        "ipc_bindings.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com:com_log",
        "//ara/com/someip:someip_data",
        "//ara/core:string_view",
        "//ara/core/model:Model_lib",
        "//communication-core/sockets:socket_ipc",
        "//ara/com/shm:shm_buffor_lib",
        "//ara/core:condition_variable",
        "//core/common:condition_lib",
    ],
)

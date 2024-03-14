
cc_library(
    name = "diag_exec_controller",
    deps = [
        "//diag/exec/data:diag_exec_header",
        "//communication-core/sockets:socket_ipc",
        "//core/logger:Logger",
        ":diag_status"
    ],
    srcs = ["exec_controller.cpp"],
    hdrs = ["exec_controller.hpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "diag_status",
    hdrs = ["status.hpp"],

)
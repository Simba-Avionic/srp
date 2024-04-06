cc_library(
    name = "diag_dtc_controller",
    srcs = ["dtc_controller.cpp"],
    hdrs = ["dtc_controller.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":diag_dtc_I_controller",
        "//core/logger:Logger",
        "//diag/dtc/data:diag_dtc_data_structure",
        "//diag/dtc/factories:diag_dtc_msg_factory",
    ],
)

cc_library(
    name = "diag_dtc_I_controller",
    hdrs = [
        "I_dtc_controller.h",
        "i_dtc.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
    ],
)

cc_library(
    name = "diag_dtc_",
    hdrs = [
        "dtc.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/dtc/controller:diag_dtc_I_controller",
    ],
)

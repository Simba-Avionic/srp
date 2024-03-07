cc_library(
    name = "dtc_service_lib",
    deps = [
        "//mw/diag/dtc/database:dtc_db",
        "//mw/diag/dtc/database:dtc_db_element",
        "//core/logger:Logger",
        "//core/application:simba_application",
        "//diag/dtc/factories:diag_dtc_msg_factory",
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
        "//diag/base/data:diag_data_structure",
        "//diag/base/data:diag_parser",
        "//diag/base/factories:diag_data_factory",
        "//diag/base/controller:diag_controller"
        ],
    srcs = ["dtcService.cpp"],
    hdrs = ["dtcService.h"],
    visibility = ["//visibility:public"],
)
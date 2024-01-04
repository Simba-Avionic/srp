cc_library(
    name = "dtc_service_lib",
    deps = [
        "//mw/diag/dtc/database:dtc_db",
        "//mw/diag/dtc/database:dtc_db_element",
        "//core/logger:Logger",
        "//diag/dtc/factories:diag_dtc_msg_factory",
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
        "//diag/base/data:diag_data_structure",
        "//diag/base/data:diag_parser",
        "//diag/base/factories:diag_data_factory",
        ],
    srcs = ["dtcService.cpp"],
    hdrs = ["dtcService.hpp","IDtcService.hpp"],
    visibility = ["//visibility:public"],
)
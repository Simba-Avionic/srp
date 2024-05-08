cc_library(
    name = "application",
    srcs = [
        "application_common.cc",
        "application_factory.cc",
    ],
    hdrs = [
        "Iapplication.h",
        "application_common.h",
        "application_factory.h",
        "parm.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/someip-controller:controller",
        "//communication-core/someip-database/code:app_config_object",
        "//communication-core/someip-database/code:someip_database_parser_lib",
        "//core/common:common_types",
        "//core/json:simba_json",
        "//core/logger:Logger",
        "//diag/exec/controller:diag_exec_controller",
        "@boost//:algorithm",
        "@com_json//:json",
    ],
)

cc_library(
    name = "simba_application",
    srcs = [
        "application_factory.cc",
        "application_mw.cc",
    ],
    hdrs = [
        "Iapplication.h",
        "application_factory.h",
        "application_mw.h",
        "application_no_ipc.h",
        "parm.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":application",
        "//communication-core/sockets:socket_ipc",
    ],
)

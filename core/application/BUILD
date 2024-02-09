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
        "//core/common:common_types",
        "//core/logger:Logger",
        "@boost//:algorithm",
        "@com_json//:json",
    ],
)

cc_library(
    name = "simba_application",
    srcs = ["application_factory.cc","application_mw.cc"],
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
        "//core/json:simba_json",
        "//diag/base/controller:diag_controller",
    ],
)

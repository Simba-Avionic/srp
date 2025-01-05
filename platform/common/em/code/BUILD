cc_binary(
    name = "em",
    srcs = [
        "em_application.cc",
        "em_application.h",
        "main.cc",
    ],
    visibility = ["//deployment/platform/em:__pkg__"],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//ara/log",
        "//core/common:condition_lib",
        "//core/common:wait_queue",
        "//core/json:simba_json",
        "//deployment/platform/em:ara",
        "//platform/common/em/code/services:IPC_Service",
        "//platform/common/em/code/services/em:em_service",
    ],
)

cc_binary(
    name = "diag_demon",
    srcs = [
        "diag_demon.cc",
        "diag_demon.h",
        "main.cc",
    ],
    visibility = ["//deployment/platform/diag_demon:__pkg__"],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//ara/log",
        "//core/common:condition_lib",
        "//core/json:simba_json",
        "//deployment/platform/diag_demon:ara",
        "//platform/common/diag_demon/code/services:example_did",
        "//platform/common/diag_demon/code/services:uds_server",
        "//platform/common/diag_demon/code/services/dtc",
    ],
)

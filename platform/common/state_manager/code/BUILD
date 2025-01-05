load("//deployment/tools/ara:adaptive.bzl", "ara_json2sm_lib")

cc_library(
    name = "diag_lib",
    hdrs = ["did_impl.h"],
    visibility = ["//platform/common/diag_demon/code:__subpackages__"],
    deps = [
        "//ara/com:com_error_domain",
        "//ara/diag:uds_lib",
        "//ara/exec/sm:sm_interfaces",
        "//ara/log",
    ],
)

ara_json2sm_lib(
    name = "sm_lib",
    src = "//deployment/system_definition/sm:example_config",
)

cc_binary(
    name = "state_manager",
    srcs = [
        "com_wrapper.h",
        "main.cc",
        "state_manager.cc",
        "state_manager.h",
    ],
    visibility = ["//deployment/platform/state_manager:__subpackages__"],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//ara/exec/sm:sm_interfaces",
        "//core/json:simba_json",
        "//deployment/platform/state_manager:ara",
        "//deployment/platform/state_manager:someip_lib",
        "//platform/common/state_manager/code:diag_lib",
        "//platform/common/state_manager/code:sm_lib",
    ],
)

cc_binary(
    name = "state_manager",
    srcs = [
        "main.cc",
        "state_manager.cc",
        "state_manager.h",
    ],
    visibility = ["//deployment/platform/state_manager:__subpackages__"],
    deps = [
        "//ara/exec:adaptive_application_lib",
        "//core/json:simba_json",
        "//deployment/platform/state_manager:ara",
        "//deployment/platform/state_manager:someip_lib",
    ],
)

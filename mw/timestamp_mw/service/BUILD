cc_binary(
    name = "timestamp_mw",
    srcs = [
        "main.cpp",
        "timestamp_service.cpp",
        "timestamp_service.hpp",
    ],
    deps = [
        "//core/timestamp:timestamp_controller",
        "//core/json:simba_json",
        "//core/common:condition_lib",
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//deployment/mw/timestamp_mw:ara",
        "//mw/timestamp_mw/ntp/controller:ntp_controller",
    ],
    visibility = ["//deployment/mw/timestamp_mw:__subpackages__"],
)
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
        "//core/common:condition_lib",
        "//deployment/platform/em:ara",
        "//platform/common/em/code/services/em:em_service",
        "//core/ntp/ntp_controller",
    ],
)

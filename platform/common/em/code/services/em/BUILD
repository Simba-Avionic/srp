cc_library(
    name = "em_service",
    srcs = ["em_service.cc"],
    hdrs = [
        "em_service.h",
    ],
    visibility = ["//platform/common/em/code:__subpackages__"],
    deps = [
        "//core/json:simba_json",
        "//ara/log",
        # "@boost//:process",
        ":em_config"
    ],
)

cc_library(
    name = "em_config",
    hdrs = ["app_config.h"],
    visibility = [
        "//mw/em/code/services/exec:__subpackages__",
        ],
)

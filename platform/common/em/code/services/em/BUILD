cc_library(
    name = "em_service",
    srcs = [
        "em_service.cc",
        "json_parser.cc",
    ],
    hdrs = [
        "em_service.h",
        "json_parser.h",
    ],
    visibility = ["//platform/common/em/code:__subpackages__"],
    deps = [
        "//core/json:simba_json",
        "//ara/log",
        ":em_db",
    ],
)

cc_library(
    name = "em_db",
    srcs = ["app_db.cc"],
    hdrs = [
        "app_config.h",
        "app_db.h",
        "i_app_db.h",
    ],
    visibility = [
        "//mw/em/code/services/exec:__subpackages__",
    ],
)

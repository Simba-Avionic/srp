cc_library(
    name = "em_service",
    srcs = ["em_service.cc"],
    hdrs = [
        "app_config.h",
        "em_service.h",
    ],
    visibility = ["//mw/em/code:__subpackages__"],
    deps = [
        "//core/json:simba_json",
        "//core/logger:Logger",
        # "@boost//:process",
    ],
)

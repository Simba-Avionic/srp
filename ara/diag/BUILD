cc_library(
    name = "monitor_lib",
    srcs = ["monitor.cc"],
    hdrs = [
        "monitor.h",
        "monitor_types.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/core:Result",
        "//ara/core:instance_specifier",
    ],
)

cc_library(
    name = "diag_error_domain",
    deps = ["//ara/core:error_lib"],
    srcs = ["diag_error_domain.cc"],
    hdrs = ["diag_error_domain.h"],
    visibility = ["//ara/diag:__subpackages__"],
)
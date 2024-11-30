cc_library(
    name = "log",
    srcs = [
        "logger.cpp",
        "logging_menager.cpp",
    ],
    hdrs = [
        "log.h",
        "logger.h",
        "logging_menager.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/log:LogStream",
        "//ara/log:common",
        "//ara/log/sink:sink_lib",
    ],
)

cc_library(
    name = "common",
    hdrs = ["common.h"],
    visibility = ["//ara/log:__subpackages__"],
)

cc_library(
    name = "LogStream",
    srcs = ["log_stream.cpp"],
    hdrs = ["log_stream.h"],
    visibility = ["//ara/log:__subpackages__"],
    deps = [
        "//ara/core:error_lib",
        "//ara/core:instance_specifier",
        "//ara/log:common",
    ],
)

################################ ONLY FOR UT ################################
cc_library(
    name = "log_for_test",
    testonly = True,
    srcs = [
        "dummy/logging_menager.cpp",
        "logger.cpp",
    ],
    hdrs = [
        "log.h",
        "logger.h",
        "logging_menager.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/log:LogStream",
        "//ara/log:common",
        "//ara/log/sink:sink_lib",
    ],
)

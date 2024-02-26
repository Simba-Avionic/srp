cc_library(
    name = "logger_interface",
    hdrs = ["ILogger.h"],
    visibility = ["//core:__subpackages__"],
    deps = ["//core/common:common_types"],
)

cc_library(
    name = "Logger",
    srcs = [
        "Logger.cpp",
        "logger_factory.cc",
    ],
    hdrs = [
        "Logger.h",
        "logger_factory.h",
    ],
    copts = ["-std=c++20"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":console_logger",
        ":logger_interface",
        "//core/common:common_types",
        "//core/results:results_lib",
        "//core/logger:dlt_logger",
    ],
)

cc_library(
    name = "console_logger",
    srcs = ["console_logger.cc"],
    hdrs = ["console_logger.h"],
    visibility = ["//core:__subpackages__"],
    deps = [
        ":logger_interface",
        "//core/common:common_types",
        "@com_github_gabime_spdlog//:spdlog",
    ],
)

cc_library(
    name = "dlt_logger",
    srcs = ["dlt_logger.cc"],
    hdrs = ["dlt_logger.h"],
    visibility = ["//core:__subpackages__"],
    deps = [
        ":logger_interface",
        "//communication-core/sockets:socket_ipc",
        "//core/common:common_types",
        "//core/common:wait_queue",
    ],
)

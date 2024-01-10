cc_library(
    name = "logger_interface",
    hdrs = ["ILogger.h"],
    visibility = ["//core:__subpackages__"],
    deps = ["//core/common:common_types"],
)

cc_library(
    name = "Logger",
    srcs = ["Logger.cpp"],
    hdrs = ["Logger.h"],
    visibility = ["//core:__subpackages__"],
    deps = [
        ":logger_interface",
        "//core/common:common_types",
    ],
    copts = ["-std=c++20"],
)

cc_library(
    name = "console_logger",
    srcs = ["console_logger.cc"],
    hdrs = ["console_logger.h"],
    visibility = ["//core:__subpackages__"],
    deps = [
        ":logger_interface",
        "//core/common:common_types",
        "@com_github_gabime_spdlog//:spdlog"
    ],
)

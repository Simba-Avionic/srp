cc_library(
    name = "sink_lib",
    srcs = [
        "console_log_sink.cpp",
        "file_log_sink.cpp",
        "log_sink.cpp",
        "remote_log_sink.cpp",
    ],
    hdrs = [
        "console_log_sink.h",
        "file_log_sink.h",
        "log_sink.h",
        "remote_log_sink.h",
    ],
    visibility = ["//ara/log:__subpackages__"],
    deps = [
        "//ara/log:LogStream",
        "//communication-core/sockets:socket_ipc",
        "//core/common:wait_queue",
    ],
)

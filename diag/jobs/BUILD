cc_library(
    name = "job_common_lib",
    srcs = ["job_common.cc"],
    hdrs = [
        "diag_request.h",
        "diag_response.h",
        "job_common.h",
    ],
    visibility = ["//diag/jobs:__subpackages__"],
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//core/logger:Logger",
        "//diag/config_controller:diag_config_controller",
    ],
)

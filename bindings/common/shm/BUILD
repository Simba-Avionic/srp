cc_library(
    name = "shm_lib",
    hdrs = [
        "shm_proxy.h",
        "shm_skeleton.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["//ara/com:com_error_domain"],
)

cc_library(
    name = "shm_buffor_lib",
    hdrs = [
        "shm_buffor_proxy.h",
        "shm_buffor_skeleton.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com:com_error_domain",
        "//ara/core:condition_variable",
    ],
)

cc_library(
    name = "timestamp_controller",
    hdrs = ["timestamp_driver.hpp"],
    srcs = ["timestamp_driver.cpp"],
    visibility = ["//visibility:public"],
    deps = [
        "@srp_platform//bindings/common/shm:shm_lib",
        "@srp_platform//ara/core:instance_specifier",
        "@srp_platform//ara/log",
        "//core/common:core",
    ]
)

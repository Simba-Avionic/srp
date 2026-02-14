i2c_drivers_deps = select({
    "//:sim": ["//core/i2c:SIMi2cdriver"],
    "//conditions:default": ["//core/i2c:i2cdriver"],
})

cc_library(
    name = "i2c_lib",
    srcs = ["i2c_service.cpp",
            "i2c_mw_read.cpp"
           ],
    hdrs = [
        "i2c_service.h",
        "i2c_mw_write.h",
        "i2c_mw_read.h",
        ],
    visibility = ["//mw/i2c_service:__subpackages__"],
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/data:i2c_factory",
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "@srp_platform//ara/diag:uds_lib",
    ] + i2c_drivers_deps, 
)
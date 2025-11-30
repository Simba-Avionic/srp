cc_library(
    name = "controller",
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/data:i2c_hdr",
        "//mw/i2c_service/data:i2c_factory",
        "@srp_platform//ara/log",
        "//mw/i2c_service/controller:i2c_controller",
    ],
    srcs = ["controller.cpp"],
    hdrs = ["controller.hpp"],
    visibility = [
        "//visibility:public",
    ],
)


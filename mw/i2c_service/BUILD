cc_library(
    name = "i2c_lib",
    srcs = ["i2c_service.cpp"],
    hdrs = ["i2c_service.h"],
    visibility = ["//mw/i2c_service:__subpackages__"],
    deps = [
        "//core/i2c:i2cdriver",
        "//communication-core/sockets:socket_ipc",
        "//core/application:simba_application",
        "//mw/i2c_service/data:i2c_factory",
        "//mw/i2c_service/data:servo_hdr",
    ],
)
cc_binary(
    name = "i2c_service",
    deps = [":i2c_lib"],
    srcs = ["main.cpp"],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

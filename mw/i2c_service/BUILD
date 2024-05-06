cc_library(
    name = "i2c_lib",
    srcs = ["i2c_service.cpp"],
    hdrs = ["i2c_service.h"],
    visibility = ["//mw/i2c_service:__subpackages__"],
    deps = [
        "//communication-core/sockets:socket_ipc",
        "//core/application:simba_application",
        "//core/i2c:i2cdriver",
        "//mw/i2c_service/data:i2c_factory",
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

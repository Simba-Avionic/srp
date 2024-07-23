cc_library(
    name = "i2cdriver",
    srcs =  select({
        "//:sim":[
            "sim_i2cdriver.cpp",
        ],
        "//conditions:default": [
            "i2cdriver.cpp",
        ],
    }),
    hdrs = [
        "i2cdriver.hpp",
        "Ii2cdriver.hpp",
        ],
    deps = [
        "//core/common:common_types",
        "//core/logger:Logger",
    ] + select({
        "//:sim":[
            "//communication-core/sockets:socket_tcp",
        ],
        "//conditions:default": [
        ],
    }),
    visibility = ["//core:__subpackages__",
    "//mw:__subpackages__",
    "//apps:__subpackages__",
    ],
)
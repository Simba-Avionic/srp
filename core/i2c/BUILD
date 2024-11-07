cc_library(
    name = "i2cdriver_base",
    hdrs = [
        "i2c_driver.hpp",
        "Ii2c_driver.hpp",
        ],
    deps = [
        "//core/common:common_types",
        "//ara/log",
    ],
    visibility = ["//core:__subpackages__",
    "//mw:__subpackages__",
    "//apps:__subpackages__",
    ],
)

cc_library(
    name = "i2cdriver",
    srcs = ["i2c_driver.cpp"],
    deps = [
        "//core/i2c:i2cdriver_base",
    ],
    visibility = ["//core:__subpackages__",
    "//mw:__subpackages__",
    "//apps:__subpackages__",
    ],
)
cc_library(
    name = "SIMi2cdriver",
    srcs = ["sim_i2c_driver.cpp"],
    deps = [
        "//core/i2c:i2cdriver_base",
    ],
    visibility = ["//core:__subpackages__",
    "//mw:__subpackages__",
    "//apps:__subpackages__",
    ],
)
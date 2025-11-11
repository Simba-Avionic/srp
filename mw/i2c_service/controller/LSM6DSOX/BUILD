cc_library(
    name = "LSM6DSOX_controller",
    deps = [
        "//mw/i2c_service/data:i2c_hdr",
        "//mw/i2c_service/data:i2c_factory",
        "@srp_platform//ara/log",
        "//mw/i2c_service/controller:i2c_controller",
        "//core/json:simba_json",
    ],
    srcs = ["controller.cpp"],
    hdrs = [
        "controller.hpp",
        "consts.hpp",    
    ],
    visibility = [
        "//apps:__subpackages__",
        "//mw/i2c_service/controller/LSM6DSOX/ut:__subpackages__",
        ],
)

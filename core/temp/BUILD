cc_library(
    name = "temp_interface",
    hdrs = [
        "ITempDriver.hpp",
        "TempDriver.hpp"
    ],
    deps = [
        "//core/common:condition_lib",
        "//ara/com:com_error_domain",
    ],
    visibility = ["//core:__subpackages__"],
)

cc_library(
    name = "temp_driver",
    srcs = ["TempDriver.cpp"],
    visibility = [
        "//core:__subpackages__",
        "//mw:__subpackages__",
        "//apps:__subpackages__"],
    deps = [
        "//core/temp:temp_interface",
        "//core/common:common_types",
        "//ara/log",
    ],
)

cc_library(
    name = "SIM_temp_driver",
    srcs = ["SIMTempDriver.cpp"],
    visibility = [
        "//core:__subpackages__",
        "//mw:__subpackages__",
        "//apps:__subpackages__"],
    deps = [
        "//core/temp:temp_interface",
        "//core/common:common_types",
        "//ara/log",
    ],
)
cc_library(
    name = "EepromController",
    deps = [
        "//core/common:common_types",
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/eeprom/factory:eeprom_factory",
    ],
    srcs = ["eepromController.cpp"],
    hdrs = ["eepromController.hpp"],
    visibility = [
        "//apps/example:__subpackages__",
        ],
)
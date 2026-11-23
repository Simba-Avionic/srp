load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")

py_library(
    name = "e220_config_lib",
    srcs = ["e220_config.py"],
    imports = ["."],
)

# E220 modem configuration from the ground station (USB-UART); see core/radio/e220/README.md.
py_binary(
    name = "e220_config",
    srcs = ["e220_config.py"],
    main = "e220_config.py",
    visibility = ["//visibility:public"],
)

py_test(
    name = "e220_config_test",
    srcs = ["e220_config_test.py"],
    main = "e220_config_test.py",
    deps = [
        ":e220_config_lib",
        "//core/radio/e220/sim:e220_sim_lib",
    ],
)

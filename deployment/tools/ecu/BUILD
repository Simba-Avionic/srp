load("@srp_platform//tools/common:flash_ecu.bzl", "flash_ecu")

flash_ecu(
    name = "flash_ec",
    file = ["//deployment/cpu/ec:pkg"],
    ip = "192.168.10.51",
    user_name = "root",
)

flash_ecu(
    name = "flash_sec_ec",
    file = ["//deployment/cpu/sec_ec:pkg"],
    ip = "192.168.10.53",
    user_name = "root",
)

flash_ecu(
    name = "flash_fc",
    file = ["//deployment/cpu/fc:pkg"],
    ip = "192.168.10.52",
    user_name = "root",
)

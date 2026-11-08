load("@srp_platform//tools/common:flash_ecu.bzl", "flash_ecu")

flash_ecu(
    name = "flash_ec",
    file = ["//deployment/cpu/ec:pkg"],
    ip = "192.168.10.51",
    tool = select({
        "//:sim": ":flash_sim",
        "//conditions:default": "@srp_platform//tools/common:flash_sh",
    }),
    user_name = "root",
)

flash_ecu(
    name = "flash_sec_ec",
    file = ["//deployment/cpu/sec_ec:pkg"],
    ip = "192.168.10.53",
    tool = select({
        "//:sim": ":flash_sim",
        "//conditions:default": "@srp_platform//tools/common:flash_sh",
    }),
    user_name = "root",
)

flash_ecu(
    name = "flash_fc",
    file = ["//deployment/cpu/fc:pkg"],
    ip = "192.168.10.52",
    tool = select({
        "//:sim": ":flash_sim",
        "//conditions:default": "@srp_platform//tools/common:flash_sh",
    }),
    user_name = "root",
)

# QEMU/sim: po flash_ec (reboot) czeka, az gosc znow odpowie na ssh.
# Uwaga: flash_ecu (srp_platform release-2503) nie przekazuje dzis parametru tool.
sh_binary(
    name = "flash_sim",
    srcs = ["flash_sim.sh"],
)

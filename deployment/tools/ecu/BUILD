load("@srp_platform//tools/common:flash_ecu.bzl", "flash_ecu")

flash_ecu(
    name = "flash_ec",
    file = ["//deployment/cpu/ec:pkg"],
    ip = "192.168.10.51",
    tool = select({
        "//:sim": "//deployment/bazel:docker_flash_sh",
        "//conditions:default": "//deployment/bazel:flash_sh",
    }),
    user_name = "root",
)

flash_ecu(
    name = "flash_sec_ec",
    file = ["//deployment/cpu/sec_ec:pkg"],
    ip = "192.168.10.53",
    tool = select({
        "//:sim": "//deployment/bazel:docker_flash_sh",
        "//conditions:default": "//deployment/bazel:flash_sh",
    }),
    user_name = "root",
)

flash_ecu(
    name = "flash_fc",
    file = ["//deployment/cpu/fc:pkg"],
    ip = "192.168.10.52",
    tool = select({
        "//:sim": "//deployment/bazel:docker_flash_sh",
        "//conditions:default": "//deployment/bazel:flash_sh",
    }),
    user_name = "root",
)

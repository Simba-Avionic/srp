load("@srp_platform//tools/common:flash_ecu.bzl", "flash_ecu")

flash_ecu(
    name = "flash_ec",
    file = ["//deployment/cpu/ec:pkg"],
    ip = "192.168.10.101",
    user_name = "root",
    tool = select({
        "//:sim": "//deployment/bazel:docker_flash_sh",
        "//conditions:default": "//deployment/bazel:flash_sh",
    }),
)

flash_ecu(
    name = "flash_fc",
    file = ["//deployment/cpu/fc:pkg"],
    ip = "192.168.10.102",
    user_name = "root",
    tool = select({
        "//:sim": "//deployment/bazel:docker_flash_sh",
        "//conditions:default": "//deployment/bazel:flash_sh",
    }),
)
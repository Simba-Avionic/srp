# Narzedzia flash uzywane w deployment/tools/ecu/BUILD.
# flash_sh: sprzet (wzor z platformy). docker_flash_sh: QEMU/sim - czeka na powrot gościa po reboocie.
alias(
    name = "flash_sh",
    actual = "@srp_platform//tools/common:flash_sh",
    visibility = ["//visibility:public"],
)

sh_binary(
    name = "docker_flash_sh",
    srcs = ["flash_sim.sh"],
    visibility = ["//visibility:public"],
)

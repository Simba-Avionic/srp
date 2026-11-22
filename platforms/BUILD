package(default_visibility = ["//visibility:public"])

# Wersje plytek (PCB) na bazie platform z srp_platform:
#   @srp_platform//bazel/platforms:bbb        - BeagleBone Black (armv7, glibc)
#   @srp_platform//bazel/platforms:rpi_zero2w - Raspberry Pi Zero 2 W (aarch64, musl)
# Platforma dziedziczy CPU/OS/toolchain po rodzicu i dokleja wersje plytki.
# Wybor kodu/konfiguracji pod konkretna plytke:
#   select({"//platforms:is_eb_v2": [...], "//conditions:default": [...]})
constraint_setting(name = "board")

constraint_value(
    name = "eb_v2",
    constraint_setting = ":board",
)

constraint_value(
    name = "lb_v1",
    constraint_setting = ":board",
)

constraint_value(
    name = "ng_v1",
    constraint_setting = ":board",
)

# --config=bbb-EB_V2
platform(
    name = "bbb_eb_v2",
    constraint_values = [":eb_v2"],
    parents = ["@srp_platform//bazel/platforms:bbb"],
)

# --config=bbb-LB_V1
platform(
    name = "bbb_lb_v1",
    constraint_values = [":lb_v1"],
    parents = ["@srp_platform//bazel/platforms:bbb"],
)

# --config=rpi-NG_V1
platform(
    name = "rpi_ng_v1",
    constraint_values = [":ng_v1"],
    parents = ["@srp_platform//bazel/platforms:rpi_zero2w"],
)

config_setting(
    name = "is_eb_v2",
    constraint_values = [":eb_v2"],
)

config_setting(
    name = "is_lb_v1",
    constraint_values = [":lb_v1"],
)

config_setting(
    name = "is_ng_v1",
    constraint_values = [":ng_v1"],
)

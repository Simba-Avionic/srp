load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")
load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application")

# The EC MavlinkDaemon (like someip_demon): the only owner of the `lora` radio link; applications with `on: mavlink` are its
# clients. The binary and the instance model come from the platform, the configuration belongs to the srp deployment:
#   /srp/opt/mavlink_daemon/mavlink_daemon_config.json   daemon configuration: system_id/component_id, driver directory,
#                                                         the `lora` link -> driver `ebyte_e220` + its configuration file,
#   /srp/opt/mavlink_daemon/ebyte_e220_lora.json         the E220 driver file (UART, M0/M1/AUX pins, AUX idle level, radio parameters),
#   /srp/opt/mavlink_daemon/drivers/libebyte_e220.so     the driver plugin (the `driver_dir` directory).
# The GPIO numbers (M0/M1/AUX), aux_idle_level and the UART in ebyte_e220_lora.json are example values from
# core/radio/e220/README.md - TO BE CONFIRMED BY THE USER ON THE HARDWARE (see README.md in this directory).
# Add both targets to the CPU package: :mavlink_daemon (binary + model + configuration) and :mavlink_daemon_driver (plugin).
adaptive_application(
    name = "mavlink_daemon",
    bin = "@srp_platform//platform/common/mavlink_daemon/code/application:mavlink_daemon",
    model_src = ["@srp_platform//platform/common/mavlink_daemon/model:instance"],
    etcs = [
        "ebyte_e220_lora.json",
        "mavlink_daemon_config.json",
    ],
    visibility = ["//deployment/cpu:__subpackages__"],
)

# The modem driver plugin: libebyte_e220.so in the daemon driver directory (`driver_dir`). Copied into the current package because
# pkg_tar from bazel_tools keeps the source package path (core/radio/e220/...) in the tar.
genrule(
    name = "ebyte_e220_so",
    srcs = ["//core/radio/e220:ebyte_e220"],
    outs = ["libebyte_e220.so"],
    cmd = "cp $< $@",
)

pkg_tar(
    name = "mavlink_daemon_driver",
    srcs = [":ebyte_e220_so"],
    package_dir = "opt/mavlink_daemon/drivers",
    visibility = ["//deployment/cpu:__subpackages__"],
)

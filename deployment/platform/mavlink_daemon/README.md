# MavlinkDaemon - EC deployment

A MavlinkDaemon instance for the EC. The binary and the model come from `srp_platform`, while the configuration belongs to the srp deployment
(like `someip_demon`): `adaptive_application` in `BUILD` packs `mavlink_daemon_config.json` and `ebyte_e220_lora.json` into
`opt/mavlink_daemon/`, and the plugin `libebyte_e220.so` goes to `opt/mavlink_daemon/drivers/` (`driver_dir` of the configuration).

| File | Content |
|------|---------|
| `mavlink_daemon_config.json` | `system_id`/`component_id`, the driver directory, the `lora` link -> driver `ebyte_e220` |
| `ebyte_e220_lora.json` | the E220 driver file: UART, M0/M1/AUX pins, `aux_idle_level`, channel, power, air rate (format: `core/radio/e220/README.md`) |

**To be confirmed by the user on the EC hardware:** the GPIO numbers M0/M1/AUX (`17`/`27`/`22`), the AUX idle level (`aux_idle_level`) and the UART
(`/dev/ttyS1`) in `ebyte_e220_lora.json` are example values.

`RadioApp` (`apps/ec/radio_service`, `/dev/ttyS1`) and `MavlinkDaemon` (`ebyte_e220_lora.json`, `/dev/ttyS1`) cannot use the same UART at the same time: the port is opened exclusively (`TIOCEXCL` + `flock`), so the second opener gets an open error.

## Building for the BeagleBone (ARM)

`bazel build --config=bbb-EB_V2 //deployment/cpu/ec:pkg` produces the ARM package (`libebyte_e220.so` and `mavlink_daemon` are
32-bit ARM EABI5 ELF files). `--config=bbb` in srp (the legacy `--crosstool_top`/`--cpu=armv7` flags, not `--platforms`) does not cross-compile
here, so use `--config=bbb-EB_V2` (or `bbb-LB_V1`) for ARM. Without it, `--define build_mode=sim` builds the host (x86) package.


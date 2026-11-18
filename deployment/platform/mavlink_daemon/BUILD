load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")
load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application")

# MavlinkDaemon EC (analogicznie do someip_demon): jedyny wlasciciel lacza radiowego `lora`; aplikacje z `on: mavlink`
# sa jego klientami. Binarka i model instancji pochodza z platformy, a config nalezy do deploymentu srp:
#   /srp/opt/mavlink_daemon/mavlink_daemon_config.json   config daemona: system_id/component_id, katalog driverow,
#                                                         lacze `lora` -> driver `ebyte_e220` + jego plik configu,
#   /srp/opt/mavlink_daemon/ebyte_e220_lora.json         plik drivera E220 (UART, piny M0/M1/AUX, parametry radia),
#   /srp/opt/mavlink_daemon/drivers/libebyte_e220.so     plugin drivera (katalog `driver_dir`).
# Numery GPIO (M0/M1/AUX) i UART w ebyte_e220_lora.json to wartosci przykladowe z core/radio/e220/README.md -
# DO POTWIERDZENIA PRZEZ USERA NA SPRZECIE (patrz README.md w tym katalogu).
# Do pakietu CPU dodaj oba targety: :mavlink_daemon (binarka + model + config) i :mavlink_daemon_driver (plugin).
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

# Plugin drivera modemu: libebyte_e220.so w katalogu driverow daemona (`driver_dir`). Kopia do biezacego pakietu, bo
# pkg_tar z bazel_tools zachowuje sciezke pakietu zrodla (core/radio/e220/...) w tarze.
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

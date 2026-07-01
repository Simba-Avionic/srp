cc_binary(
    name = "SecEnvService",
    srcs = [
        "main.cpp",
        "env_service.cpp",
        "env_service.hpp",
        "service.hpp",
    ],
    visibility = [
        "//deployment/apps/sec_ec/env_service:__subpackages__",
    ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//mw/temp/controller:temp_controller_mw",
        "//deployment/apps/sec_ec/env_service:ara",
        "//deployment/apps/sec_ec/env_service:someip_lib",
        "//communication-core/sockets:socket_ipc",
        "//mw/i2c_service/controller/adcsensor:adc_sensor_controller",
        "//mw/i2c_service/controller/24lc32at/eeprom_config:eeprom_config",
        "//core/json:simba_json",
    ],
)

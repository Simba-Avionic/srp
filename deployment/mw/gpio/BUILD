load("//deployment/tools/ara:adaptive_application.bzl", "ara_runtime_lib", "adaptive_application")

filegroup(
    name = "instance",
    srcs = [
        "app_config.json",
        "//deployment/system_definition/diag/jobs/gpio_mw:gpio_mw_job",
    ],
    visibility = ["//mw/gpio_server:__subpackages__"],
)
ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/mw/gpio:instance"],
    visibility = ["//mw/gpio_server:__subpackages__"],
)

adaptive_application(
    name = "gpio_service",
    bin = "//mw/gpio_server:gpio_service",
    model_src = ["//deployment/mw/gpio:instance"],
    etcs = ["config.json"],
    visibility = ["//deployment/cpu/ec:__subpackages__"],
)
load("//deployment/tools/ara:adaptive_application.bzl", "ara_runtime_lib", "adaptive_application")
cc_library(
    name = "gpio_service_lib",
    srcs = [
        "gpio_mw.cpp",
        ],
    hdrs=[
        "gpio_mw.hpp",
        "gpio_mw_did.h",
    ],
    deps = [
        "//core/gpio:gpio_drivers",
        "//mw/gpio_server/data:gpio_hdr",
        "//core/json:simba_json",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//diag/jobs/skeleton:did_job_lib",
    ],
    visibility = [
        "//mw/gpio_server/tests:__subpackages__",
        ],
)

cc_binary(
    name = "gpio_service",
    deps = [
        ":gpio_service_lib",
        "//mw/gpio_server:ara",
        ],
    srcs = [
        "main.cpp",
        ],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/mw/gpio:instance"]
)


adaptive_application(
    name = "gpioService",
    bin = "//mw/gpio_server:gpio_service",
    model_src = ["//deployment/mw/gpio:instance"],
    visibility = ["//visibility:public"],
)
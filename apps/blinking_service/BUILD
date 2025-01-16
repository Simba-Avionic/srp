cc_binary(
    name = "blinking_service",
    deps = [
        "//deployment/apps/blinking_service:ara",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//diag/jobs/skeleton:did_job_lib",
        "//mw/gpio_server/controller:gpio_controller",
    ],
    srcs = ["blinking_service.cpp", "main.cpp", "blinking_service.hpp", "diode_controller.hpp", "diode_controller.cpp","diode_did.hpp", "diode_did.cpp"],
    visibility = ["//deployment/apps/blinking_service:__subpackages__"],
)

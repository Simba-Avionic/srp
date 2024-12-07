

cc_library(
    name = "primer_controller",
    deps = [
        "//mw/gpio_server/controller:gpio_controller",
        "//core/common:condition_lib",
        "//core/json:simba_json",
    ],
    srcs = [
        "primer_controller.cpp",
    ],
    hdrs = [
        "primer_controller.hpp",
    ],
    visibility = ["//apps/ec/primer_service:__subpackages__"],
)

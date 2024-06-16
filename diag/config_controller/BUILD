cc_library(
    name = "diag_config_controller",
    srcs = [],
    hdrs = ["config_controller.h"],
    visibility = ["//visibility:public"],
    deps = ["//core/common:common_types"],
)

cc_library(
    name = "diag_config_parser",
    srcs = ["config_controller.cc"],
    hdrs = ["config_controller_json.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/config_controller:diag_config_controller",
        "@com_json//:json",
    ],
)

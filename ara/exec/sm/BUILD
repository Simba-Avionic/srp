cc_library(
    name = "sm_interfaces",
    hdrs = [
        "i_sm_com_wrapper.h",
        "i_state.h",
        "i_state_controller.h",
    ],
    visibility = [
        "//ara/exec/sm:__subpackages__",
        "//platform/common/state_manager/code:__subpackages__",
    ],
)

cc_library(
    name = "state_meager_lib",
    srcs = ["state_controller.cc"],
    hdrs = [
        "state.h",
        "state_controller.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["//ara/exec/sm:sm_interfaces"],
)

filegroup(
    name = "sm",
    srcs = ["initialization.h"],
    visibility = ["//visibility:public"],
)

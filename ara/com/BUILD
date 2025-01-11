cc_library(
    name = "com_error_domain",
    srcs = [
        "com_error_domain.cc",
        "someip_error.cc",
    ],
    hdrs = [
        "com_error_domain.h",
        "someip_error.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com/someip:someip_enums",
        "//ara/core:error_lib",
    ],
)

cc_library(
    name = "com_log",
    hdrs = ["log.h"],
    visibility = ["//ara/com:__subpackages__"],
    deps = ["//ara/log"],
)

cc_library(
    name = "com",
    srcs = ["initialization.cc"],
    hdrs = ["initialization.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com:com_controller",
        "//ara/com/someip/controller",
        "//ara/core:Result",
        "//ara/diag:diagnostic_menager",
    ],
)

cc_library(
    name = "com_controller",
    srcs = ["com_controller.cc"],
    hdrs = [
        "com_controller.h",
        "i_com_client.h",
    ],
    visibility = ["//ara:__subpackages__"],
    deps = [
        "//ara/com:com_error_domain",
        "//ara/com:com_log",
        "//bindings/common/socket:ipc_socket",
        "//ara/core:Result",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    visibility = ["//ara/com:__subpackages__"],
)
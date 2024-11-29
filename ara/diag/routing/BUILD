cc_library(
    name = "routing",
    srcs = ["routable_uds_service.cc"],
    hdrs = ["routable_uds_service.h"],
    visibility = ["//ara/diag:__subpackages__"],
    deps = [
        "//ara/com/socket:socket_ipc",
        "//ara/core:Result",
        "//ara/core:instance_specifier",
        "//ara/core:model_db_lib",
        "//ara/diag:diag_error_domain",
        "//ara/log",
    ],
)

################################ ONLY FOR UT ################################
cc_library(
    name = "routing_for_ut",
    testonly = True,
    srcs = ["routable_uds_service.cc"],
    hdrs = ["routable_uds_service.h"],
    visibility = ["//ara/diag:__subpackages__"],
    deps = [
        "//ara/com/socket:socket_ipc",
        "//ara/core:Result",
        "//ara/core:instance_specifier",
        "//ara/core:model_db_lib",
        "//ara/diag:diag_error_domain",
        "//ara/log:log_for_test",
    ],
)

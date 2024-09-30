cc_library(
    name = "connector",
    srcs = ["service_connector.cc"],
    hdrs = ["service_connector.h"],
    visibility = ["//platform/common/someip_demon/code:__subpackages__"],
    deps = [
        "//ara/com/socket:ipc_socket",
        "//ara/com/someip:someip_data",
        "//ara/log",
        "//core/common:condition_lib",
        "//platform/common/someip_demon/code/common:udp_lib",
        "//platform/common/someip_demon/code/connector:internal",
        "//platform/common/someip_demon/code/db",
        "//platform/common/someip_demon/code/sd:sd_controller",
    ],
)

cc_library(
    name = "internal",
    srcs = [
        "common_connector.cc",
        "proxy_connector.cc",
        "sd_connector.cc",
        "skeleton_connector.cc",
    ],
    hdrs = [
        "common_connector.h",
        "i_connector.h",
        "i_db.h",
        "proxy_connector.h",
        "sd_connector.h",
        "skeleton_connector.h",
    ],
    visibility = ["//platform/common/someip_demon/code/connector:__pkg__"],
    deps = [
        "//ara/com/socket:ipc_socket",
        "//ara/com/someip:someip_data",
        "//ara/log",
        "//core/common:condition_lib",
        "//platform/common/someip_demon/code/common:udp_lib",
        "//platform/common/someip_demon/code/db",
    ],
)

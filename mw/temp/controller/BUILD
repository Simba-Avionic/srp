cc_library(
    name = "temp_controller_mw",
    srcs = [
        "temp_controller.h",
        "temp_controller.cpp",
    ],
    visibility = [
        "//deployment:__subpackages__",
        "//apps/env_service:__subpackages__",
        "//mw/temp:__subpackages__",
        ],
    
    deps = [
        "//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type",
        "//communication-core/sockets:socket_interface",
        "//mw/temp/subscribe_msg:subscribe_header",
        "//mw/temp/subscribe_msg:subscribe_msg_factory",
        "//mw/temp/temp_reading_msg:temp_reading_msg_factory",
        "//mw/temp/service:temp_service_mw",
        "@com_json//:json"

    ],
    data = []
)
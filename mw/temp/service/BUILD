cc_binary(
    name = "temp_service_mw",
    srcs = [
        "main.cc",
        "temp_service.h",
        "temp_service.cpp",
    ],
    visibility = [
        "//deployment:__subpackages__",
        "//mw/temp:__subpackages__"
    ],
    
    deps = [
        "//core/application:simba_application",
        "//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type",
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
        "//mw/temp/subscribe_msg:subscribe_header",
        "//mw/temp/subscribe_msg:subscribe_msg_factory",
        "//mw/temp/temp_reading_msg:temp_reading_msg_factory",
        "//core/json:simba_json",

    ],
)
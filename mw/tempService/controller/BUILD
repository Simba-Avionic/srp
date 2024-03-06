cc_binary(
    name = "tempControllerMW",
    srcs = [
        "main.cc",
        "TempController.h",
        "TempController.cpp",
    ],
    visibility = ["//deployment:__subpackages__"],
    
    deps = [
        "//core/application:simba_application",
        "//communication-core/network-data:network_data_structure",
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
        "//mw/tempService/subscribe_msg:subscribe_header",
        "//mw/tempService/subscribe_msg:subscribe_msg_factory",

    ],
    data=["logger.json","app.json","srp_app.json"]
)
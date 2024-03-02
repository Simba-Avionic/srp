cc_binary(
    name = "tempControllerMW",
    srcs = [
        "main.cc",
        "TempController.cpp",
        "TempController.h"
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
        "//diag/base/data:diag_data_structure",
        # dodatkowe zależności jeśli potrzeba
    ],
    data=["logger.json","app.json","srp_app.json"]
)

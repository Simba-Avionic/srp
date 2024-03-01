cc_binary(
    name = "tempServiceMW",
    srcs = [
        "main.cc",
        "TempService.cpp",
        "TempService.hpp"
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
        "//diag/base/data:diag_data_structure"
        # dodatkowe zależności jeśli potrzeba
    ],
)

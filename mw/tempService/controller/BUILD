cc_binary(
    name = "tempControllerMW",
    srcs = [
        "main.cpp",
        "TempController.cpp",
        "TempController.h"
    ],
    visibility = ["//deployment:__subpackages__"],
    
    deps = [
        "//core/application:simba_application",
        "//communication-core/network-data:network_data_structure",
        "//communication-core/sockets:socket_interface",
        "//communication-core/sockets:socket_ipc",
        # "//communication-core/network-data:network_data_type",
        # "//diag/base/data:diag_data_structure",
    ],
    # data=["logger.json","app.json","srp_app.json"]
)

# cc_binary(
#     name = "tempControllerMW2",
#     srcs = [
#         "main3.cpp",
#         # "TempController.cpp",
#         # "TempController.h"
#     ],
#     visibility = ["//deployment:__subpackages__"],
    
#     deps = [
#         "//core/application:simba_application",
#         "//communication-core/network-data:network_data_structure",
#         "//communication-core/sockets:socket_interface",
#         "//communication-core/sockets:socket_ipc",
#     ],
#     # data=["logger.json","app.json","srp_app.json"]
# )
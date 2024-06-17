cc_library(
    name = "mock_socket",
    hdrs = [
        "mockSocket.h"
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//communication-core/sockets:socket_interface",
    ],
    testonly = True,
)

cc_library(
    name = "mock_udp_multicast_socket",
    hdrs = [
        "mock_usp_multicast_socket.h"
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//communication-core/sockets:socket_interface",
    ],
    testonly = False,
)
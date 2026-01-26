cc_library(
    name = "temp_controller_mw",
    srcs = [
        "temp_controller.h",
        "temp_controller.cpp",
    ],
    visibility = [
        "//deployment:__subpackages__",
        "//apps/ec/env_service:__subpackages__",
        "//apps/fc/env_service:__subpackages__",
        "//mw/temp:__subpackages__",
        ],
    
    deps = [
        "//communication-core/sockets:socket_interface",
        "//mw/temp/service:temp_service_mw",
        "@com_json//:json",
        "//mw/temp/hdr:temp_com_data",
    ],
)
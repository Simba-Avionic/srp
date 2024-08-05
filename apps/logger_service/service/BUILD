cc_binary(
    name = "logger_service",
    srcs = [
        "logger_service.cpp",
        "logger_service.hpp",
        "main.cc",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//diag/dtc/controller:diag_dtc_",
        "//apps/logger_service/csvdriver:csvdriver",
    ],
)

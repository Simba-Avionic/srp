cc_binary(
    name = "primer",
    srcs = [
        "primer_service.cpp",
        "primer_service.hpp",
        "main.cc",
    ],
    visibility = ["//deployment:__subpackages__"],
    deps = [
        "//core/application:simba_application",
        "//communication-core/someip-controller:proxy",
        "//communication-core/someip-controller:skeleton",
        "//diag/dtc/controller:diag_dtc_",
    ],
)

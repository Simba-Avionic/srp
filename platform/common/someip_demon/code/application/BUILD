load("//deployment/tools/ara:adaptive_application.bzl", "ara_runtime_lib")

ara_runtime_lib(
    name = "ara",
    model_src = ["//deployment/platform/someip_demon:instance"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "someip_demon",
    srcs = [
        "main.cc",
        "someip_demon.cc",
        "someip_demon.h",
    ],
    visibility = ["//deployment/platform/someip_demon:__pkg__"],
    deps = [
        ":ara",
        "//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//core/json:simba_json",
        "//platform/common/someip_demon/code/common/com/impl:multicast_controller",
        "//platform/common/someip_demon/code/sd:sd_controller",
        "//platform/common/someip_demon/code/common:controller",
        "//platform/common/someip_demon/code/connector",
        "//platform/common/someip_demon/code/db",
    ],
)

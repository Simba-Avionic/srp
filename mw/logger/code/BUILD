cc_binary(
    name = "dlt_server",
    srcs = ["main.cc"],
    visibility = ["//deployment/mw/loger/dlt_fc:__pkg__","//deployment/mw/loger/dlt_fc/fc:__pkg__"],
    deps = [
        "//core/application:simba_application",
        "//mw/logger/code/application:dlt_service_lib",
    ],
)

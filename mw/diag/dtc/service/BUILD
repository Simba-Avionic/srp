cc_library(
    name = "dtc_service_lib",
    srcs = ["dtcService.cpp"],
    hdrs = ["dtcService.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/application:simba_application",
        "//core/logger:Logger",
        "//mw/diag/dtc/service:db_lib",
        "//mw/diag/dtc/service/diag:uds_jobs",
        "//core/json:simba_json",
    ],
)

cc_library(
    name = "db_lib",
    srcs = ["database.cc"],
    hdrs = ["database.h"],
    visibility = ["//mw/diag/dtc:__subpackages__"],
    deps = [],
)

cc_library(
    name = "someip_database_lib",
    srcs = ["database.cc"],
    hdrs = [
        "database.h",
        "endpoint.h",
        "interface.h",
        "service.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//core/common:common_types",
        "//core/results:results_lib",
    ],
)

cc_library(
    name = "someip_database_parser_lib",
    srcs = [],
    hdrs = [
        "database_parser.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/someip-database/code:someip_database_lib",
        "//core/common:common_types",
        "//core/results:results_lib",
        "@com_json//:json",
    ],
)

cc_library(
    name = "config_database_lib_p",
    deps = [],
    srcs = ["config_db.cc"],
    hdrs = ["config_db.h"],
    visibility = ["//communication-core/someip-database:__subpackages__"],
)
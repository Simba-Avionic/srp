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
        "@com_json//:json",
    ],
)

cc_library(
    name = "config_database_lib_p",
    srcs = ["config_db.cc"],
    hdrs = ["config_db.h"],
    visibility = ["//communication-core/someip-database:__subpackages__"],
    deps = [
        "//communication-core/someip-database/code:someip_database_lib",
        "//core/common:common_types",
    ],
)

cc_library(
    name = "config_database_parser_lib",
    srcs = [],
    hdrs = [
        "config_db_parser.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/someip-database/code:config_database_lib_p",
        "//core/common:common_types",
        "@com_json//:json",
    ],
)

cc_library(
    name = "app_config_object",
    hdrs = ["com_config.h"],
    srcs = ["com_config.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/someip-database/code:config_database_lib_p",
        "//communication-core/someip-database/code:config_database_parser_lib",
        "@com_json//:json",
        "//core/logger:Logger",
    ],
)

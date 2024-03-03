cc_test(
    name = "database_parser_test",
    srcs = ["database_parser_test.cc"],
    data = ["//communication-core/someip-database/test/resource:files"],
    visibility = [],
    deps = [
        "//communication-core/someip-database/code:someip_database_parser_lib",
        "@com_json//:json",
        "@com_google_googletest//:gtest_main",
    ],
)

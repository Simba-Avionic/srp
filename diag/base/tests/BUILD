cc_test(
    name = "data_structure",
    srcs = ["test_data_structure.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/base/data:diag_data_structure",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "parser",
    srcs = ["test_parser.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/base/data:diag_data_structure",
        "//diag/base/data:diag_parser",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "diag_data_factory",
    srcs = ["test_diag_data_factory.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/base/data:diag_data_structure",
        "//diag/base/factories:diag_data_factory",
        "@com_google_googletest//:gtest_main",
    ],
)

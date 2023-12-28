cc_test(
    name = "data_structure",
    srcs = ["test_data_structure.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/data:diag_data_structure",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "diag_msg_factory",
    srcs = ["test_diag_msg_factory.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//diag/data:diag_data_structure",
        "//diag/factories:diag_msg_factory",
        "@com_google_googletest//:gtest_main",
    ],
)

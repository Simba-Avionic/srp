
# cc_test(
#     name = "temp_controller",
#     srcs = ["test_temp_controller.cc"],
#     visibility = ["//visibility:public"],
#     deps=[
#         "@com_google_googletest//:gtest_main",
#         "//mw/temp/controller:temp_controller_mw",
#         "//communication-core/sockets/mock:mock_socket",
#     ]
# )

cc_test(
    name = "temp_register_test",
    srcs = ["test_temp_register.cc"],
    deps = [
        "@com_google_googletest//:gtest_main",
        "//mw/temp/controller:temp_controller_mw",
        "@srp_platform//ara/log",
        "//communication-core/sockets:socket_ipc",
    ],
)

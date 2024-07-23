cc_test(
    name = "subscribe_msg_factory",
    srcs = [
        "test_subscribe_msg_factory.cpp",
        "test_subscribe_msg.cpp"
    ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/temp/subscribe_msg:subscribe_header",
        "//mw/temp/subscribe_msg:subscribe_msg_factory",
    ]
)

cc_test(
    name = "temp_reading_msg_factory",
    srcs = [
        "test_temp_reading_msg_factory.cpp",
        "test_subscribe_msg.cpp",
        "test_subscribe_msg_factory.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/temp/temp_reading_msg:temp_reading_msg_factory",
        "//mw/temp/subscribe_msg:subscribe_header",
        "//mw/temp/subscribe_msg:subscribe_msg_factory"
    ]
)

cc_test(
    name = "temp_controller",
    srcs = ["test_temp_controller.cc"],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/temp/controller:temp_controller_mw",
        "//communication-core/sockets/mock:mock_socket",
        "//core/logger:Logger",
    ]
)
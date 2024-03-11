cc_test(
    name = "subscribe_msg",
    srcs = ["test_subscribe_msg.cpp"],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/tempService/subscribe_msg:subscribe_header",
    ]
)

cc_test(
    name = "subscribe_msg_factory",
    srcs = ["test_subscribe_msg_factory.cpp"],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/tempService/subscribe_msg:subscribe_header",
        "//mw/tempService/subscribe_msg:subscribe_msg_factory",
    ]
)

cc_test(
    name = "temp_reading_msg_factory",
    srcs = ["test_temp_reading_msg_factory.cpp"],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/tempService/temp_reading_msg:temp_reading_msg",
        "//mw/tempService/temp_reading_msg:temp_reading_msg_factory",
    ]
)
cc_test(
    name = "data_structure",
    srcs = [
        "test_header.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/gpio_server/data:gpio_hdr"
    ]
)

cc_test(
    name = "gpio_controller",
    srcs = [
        "test_controller.cpp",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/gpio_server/controller:gpio_controller",
        "//communication-core/sockets/mock:mock_socket",
    ]
)

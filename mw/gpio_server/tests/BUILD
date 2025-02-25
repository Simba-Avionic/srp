cc_test(
    name = "data_structure_test",
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
    name = "gpio_controller_test",
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

cc_test(
    name = "gpio_service_test",
    srcs = [
        "test_service.cc",
        ],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//mw/gpio_server:gpio_service_lib",
        "//core/gpio/mock:mock_gpio",
        "//communication-core/sockets/mock:mock_socket",
        "//mw/gpio_server/data:gpio_hdr",
    ],
    data = [
        "//mw/gpio_server/tests/test_data:gpio_test_data",
        ],
)

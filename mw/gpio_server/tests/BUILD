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

cc_test(
    name = "gpio_service",
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
)

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "communication_core",
    visibility = ["//visibility:public"],
    deps = [
        "//communication-core/network-data:interfaces",
        "//communication-core/network-data:network_data_structure",
        "//communication-core/network-data:network_data_type",
        "//communication-core/sockets:socket_ipc",
        "//communication-core/someip:someip_header",
        "//communication-core/someip:someip_types",
        "//communication-core/someip-controller:someip_controller",
        "//communication-core/someip/factory:someip_header_factory",
        "//communication-core/someip/factory:someip_message_factory",
        "//communication-core/sockets:socket_udp",
    ],
)

cc_test(
    name = "com_core_test",
    srcs = ["test/sample_test.cc"],
    deps = ["@com_google_googletest//:gtest_main"],
)

#bazel run :benchmark
cc_binary(
    name = "com_core_benchmark",
    srcs = ["test/benchmark/sample_benchmark.cc"],
    deps = [
        "@com_google_benchmark//:benchmark_main",
    ],
)

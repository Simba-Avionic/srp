cc_test(
    name = "test_shm",
    srcs = [
        "shm_buffor_proxy_test.cc",
        "shm_buffor_skeleton_test.cc",
        "shm_proxy_test.cc",
        "shm_skeleton_test.cc",
    ],
    features = ["test_target"],
    deps = [
        "//ara/core:instance_specifier",
        "//bindings/common/shm:shm_buffor_lib",
        "//bindings/common/shm:shm_lib",
        "@com_google_googletest//:gtest_main",
    ],
)

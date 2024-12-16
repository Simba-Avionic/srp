cc_test(
    name = "test_shm",
    srcs = [
        "shm_skeleton_test.cc",
        "shm_proxy_test.cc",
        "shm_buffor_skeleton_test.cc",
        "shm_buffor_proxy_test.cc",
    ],
    features = ["test_target"],
    deps = [
        "//ara/com/shm:shm_lib",
        "@com_google_googletest//:gtest_main",
        "//ara/core:instance_specifier",
        "//ara/com/shm:shm_buffor_lib",
    ],
)

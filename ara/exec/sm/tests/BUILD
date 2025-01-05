load("//deployment/tools/ara:adaptive.bzl", "ara_json2sm_lib")
cc_library(
    name = "mock",
    testonly = True,
    srcs = [],
    hdrs = [
        "com_wrapper_mock.h",
        "state_mock.h",
    ],
    visibility = ["//ara/exec/sm/tests:__pkg__"],
    deps = [
        "//ara/exec/sm:sm_interfaces",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "State_controller_test",
    srcs = ["test_state_controller.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/exec/sm:state_meager_lib",
        "//ara/exec/sm/tests:mock",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "Generated_state_controller_test",
    srcs = ["test_generated_state_controller.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/exec/sm/tests:sm_lib",
        "//ara/exec/sm/tests:mock",
        "@com_google_googletest//:gtest_main",
    ],
)

filegroup(
    name = "sm_files",
    srcs = ["example_sm_model.json"],
)

ara_json2sm_lib(
    name = "sm_lib",
    src = "//ara/exec/sm/tests:sm_files",
    visibility = ["//ara/exec/sm/tests:__pkg__"]
)
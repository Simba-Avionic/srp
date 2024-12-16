cc_library(
    name = "file_lib",
    srcs = ["file.cpp"],
    visibility = [
        "//core/gpio:__subpackages__",
        ],
    deps = [
        "//ara/log",
        "//core/file:file_interface",
    ],
)

cc_library(
    name = "file_interface",
    hdrs = [
         "file.hpp",
    ],
    visibility = [
        "//core/gpio:__subpackages__",
        ],
)

cc_library(
    name = "mock_file",
    hdrs = ["mock_file.hpp"],
    visibility = ["//visibility:public"],
    deps=[
        "@com_google_googletest//:gtest_main",
        "//core/file:file_interface",
    ],
    testonly = True,
)

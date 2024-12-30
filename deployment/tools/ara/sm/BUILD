load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "sm_lib",
    srcs = [
        "config_extractor.py",
        "cpp_extractor.py",
        "db.py",
        "sm_parser.py",
        "state.py",
    ],
    visibility = ["//deployment/tools/ara:__subpackages__"],
    deps = [
        "//deployment/tools/ara/common:common_lib"
    ],
)

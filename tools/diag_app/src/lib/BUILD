load("@rules_python//python:defs.bzl", "py_library")
load("@pip_deps//:requirements.bzl", "all_requirements")

py_library(
    name = "web_lib",
    srcs = glob(["*.py"]),
    deps = [
        "@doipclient",
    ] + all_requirements,
    visibility = ["//tools/diag_app/src:__subpackages__"],
)

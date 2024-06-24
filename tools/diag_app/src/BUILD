load("@rules_python//python:defs.bzl", "py_binary")
load("@pip_deps//:requirements.bzl", "all_requirements")
load("//deployment/bazel:web_config.bzl","web_config")

web_config(
    name="config",
    files=["//deployment/cpu/ec:pkg_config","//deployment/cpu/fc:pkg_config"],
)

py_binary(
    name = "main",
    srcs = ["main.py"],
    args = ["//tools/diag_app/src:config"],
    data = ["//tools/diag_app/src:config"],
    deps = [
        "//tools/diag_app/src/lib:web_lib",
        "@doipclient",
    ] + all_requirements,
)

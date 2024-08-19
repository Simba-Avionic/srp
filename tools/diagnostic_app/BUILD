load("@rules_python//python:defs.bzl", "py_binary")
load("@pip_deps//:requirements.bzl", "all_requirements")

py_binary(
    name = "app",
    srcs = ["app.py"],
    deps = [
        # "//third_party/python:flask",
        "@doipclient",
        # "@flask_socketio",
    ]+all_requirements,
    data = ["//tools/diagnostic_app/js","//tools/diagnostic_app/templates:html"],
)
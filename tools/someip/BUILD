load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "test",
    srcs = ["test.py","addition_method_parameters.py","temperature_msg.py"],
    imports = ["."],
    deps = [
        "//third_party/python/someipy/src:someipy_lib"
    ],
)
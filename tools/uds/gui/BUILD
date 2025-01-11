load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "uds_app",
    srcs = [
        "connect_window.py",
        "console.py",
        "popup_window.py",
        "singleton_lib.py",
        "uds_app.py",
        "uds_con.py",
        "machine_state_window.py"
    ],
    deps = [
        "@doipclient",
    ],
)

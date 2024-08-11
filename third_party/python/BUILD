load("@rules_python//python:defs.bzl", "py_library")
load("@pip_deps//:requirements.bzl", "requirement")


package(default_visibility = ["//visibility:public"])

# py_library(
#     name = "flask",
#     deps = [
#         requirement("Flask"),
#         requirement("Jinja2"),
#         requirement("Werkzeug"),
#         requirement("itsdangerous"),
#         requirement("click"),
#         requirement("MarkupSafe"),
#         requirement("multidict"),
#         requirement("aiohttp"),
#         requirement("async-timeout"),
#         requirement("multidict"),
#     ],
# )

py_library(
    name = "pyfranca",
    deps = [
        requirement("pyfranca"),
        requirement("ply"),
    ],
)

# py_library(
#     name = "pytest",
#     deps = [
#         requirement("pytest"),
#         requirement("pluggy"),
#         requirement("py"),
#         requirement("packaging"),
#         requirement("importlib-metadata"),
#         requirement("more-itertools"),
#     ],
# )
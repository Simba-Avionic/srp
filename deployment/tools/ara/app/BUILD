load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "app_lib",
    srcs = [
        "adaptive_application.py",
        "adaptive_application_db.py",
        "adaptive_application_parser.py",
        "adaptive_application_extractor.py",
        "adaptive_application_runtime_env.py"
    ],
    visibility = ["//deployment/tools/ara:__subpackages__"],
    deps = [
        "//deployment/tools/ara/common:common_lib",
        "//deployment/tools/ara/common:data_structure_lib",
    ],
)

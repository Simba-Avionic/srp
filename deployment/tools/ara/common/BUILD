load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "data_structure_lib",
    srcs = [
        "data_structure.py",
        "data_structure_db.py",
        "data_structure_extractor.py",
        "data_structure_parser.py",
    ],
    visibility = ["//deployment/tools/ara:__subpackages__"],
    deps = [":common_lib"],
)

py_library(
    name = "common_lib",
    srcs = [
        "singleton_lib.py",
    ],
    visibility = ["//deployment/tools/ara:__subpackages__"],
)

py_library(
    name = "common_parser",
    srcs = ["common_parser.py"],
    visibility = ["//deployment/tools/ara:__subpackages__"],
    deps = [
        "//deployment/tools/ara/app:app_lib",
        "//deployment/tools/ara/common:data_structure_lib",
        "//deployment/tools/ara/diag:diag_lib",
        "//deployment/tools/ara/someip/lib:someip_lib",
    ],
)

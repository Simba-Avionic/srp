load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "someip_gen",
    srcs = ["someip_gen.py"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
)

py_binary(
    name = "json2config",
    srcs = ["json2config.py"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "json2runtime_env",
    srcs = ["json2runtime_env.py"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "json2someip_lib",
    srcs = ["json2someip_lib.py"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "data_structure_gen",
    srcs = ["data_structure_gen.py"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "example_res",
    srcs = [
        "example_app.json",
        "example_data_type.json",
        "example_service.json",
    ],
)

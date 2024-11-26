load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "someip_gen",
    srcs = ["someip_gen.py"],
    args = ["deployment/tools/ara/example_app.json"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
)

py_binary(
    name = "json2config",
    srcs = ["json2config.py"],
    args = ["deployment/tools/ara/example_app.json"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "json2runtime_env",
    srcs = ["json2runtime_env.py"],
    args = ["deployment/tools/ara/example_app.json"],
    data = ["//deployment/tools/ara:example_res"],
    deps = [
        "//deployment/tools/ara/common:common_parser",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "json2someip_lib",
    srcs = ["json2someip_lib.py"],
    args = ["deployment/tools/ara/example_app.json"],
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

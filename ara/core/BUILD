cc_library(
    name = "instance_specifier",
    srcs = ["instance_specifier.cc"],
    hdrs = ["instance_specifier.h"],
    visibility = ["//ara:__subpackages__"],
    deps = [
        "//ara/core:Result",
        "//ara/core:core_error_lib",
        "//ara/core:error_lib",
        "//ara/core:string_view",
    ],
)

cc_library(
    name = "Result",
    hdrs = ["result.h"],
    visibility = ["//ara:__subpackages__"],
)

cc_library(
    name = "string_view",
    hdrs = ["string_view.h"],
    visibility = ["//ara:__subpackages__"],
)

cc_library(
    name = "core_error_lib",
    srcs = [
        "core_error_domain.cc",
    ],
    hdrs = [
        "core_error_domain.h",
    ],
    visibility = ["//ara/core:__subpackages__"],
    deps = ["//ara/core:error_lib"],
)

cc_library(
    name = "error_lib",
    srcs = [
        "core_error_domain.cc",
        "error_domain.cc",
    ],
    hdrs = [
        "core_error_domain.h",
        "error_code.h",
        "error_domain.h",
        "exception.h",
    ],
    visibility = ["//ara:__subpackages__"],
    deps = ["//ara/core:string_view"],
)

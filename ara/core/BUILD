cc_library(
    name = "instance_specifier",
    srcs = ["instance_specifier.cc"],
    hdrs = ["instance_specifier.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/core:Result",
        "//ara/core:core_error_lib",
        "//ara/core:error_lib",
        "//ara/core:string_view",
    ],
)

cc_library(
    name = "Result",
    hdrs = [
        "result.h",
    ],
    visibility = ["//visibility:public"],
    deps = ["//ara/core:error_lib"],
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
        "error_domain.cc",
    ],
    hdrs = [
        "error_code.h",
        "error_domain.h",
        "exception.h",
    ],
    visibility = ["//ara:__subpackages__"],
    deps = ["//ara/core:string_view"],
)

cc_library(
    name = "condition_variable",
    srcs = ["condition_variable.cc"],
    hdrs = [
        "condition_variable.h",
        "condition_variable_type.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/core:core_error_lib",
        "//ara/core:instance_specifier",
        "//bindings/common/shm:shm_lib",
    ],
)

cc_library(
    name = "ara_initialization_lib",
    hdrs = ["initialization.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/core:Result",
        "//ara/core:model_db_lib",
    ],
)

cc_library(
    name = "model_db_lib",
    srcs = ["model_item.cc"],
    hdrs = [
        "model_db.h",
        "model_item.h",
    ],
    visibility = [
        "//ara:__subpackages__",
        "//diag/jobs:__subpackages__",
    ],
    deps = [
        "//ara/core:core_error_lib",
        "//ara/core:instance_specifier",
        "//ara/core/model:Model_lib",
    ],
)

cc_library(
    name = "diag_data_structure",
    srcs = ["data_structure.cc"],
    hdrs = ["data_structure.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "diag_parser",
    srcs = ["parser.cc"],
    hdrs = ["parser.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//core/results:results_lib",
        "//diag/base/data:diag_data_structure",
    ],
)

cc_library(
    name = "diag_dtc_controller",
    srcs = ["dtc_controller.cpp"],
    hdrs = ["dtc_controller.hpp"],
    deps=[
        ":diag_dtc_I_controller",
        ":diag_dtc_msg_factory",
        ":diag_dtc_data_structure"
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "diag_dtc_I_controller",
    hdrs = ["I_dtc_controller.hpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "dtc_msg_factory",
    deps = [
        "//communication-core:communication_core",
        ":dtc_header"
        ],
    srcs = ["msg/dtc_msg_factory.cpp"],
    hdrs = ["msg/dtc_msg_factory.hpp"],
    visibility = ["//srp/mw/dtc:__subpackages__"],
)

cc_library(
    name = "dtc_header",
    deps = [
        "//communication-core:communication_core"
        ],
    srcs = ["msg/dtc_header.cpp"],
    hdrs = ["msg/dtc_header.hpp"],
    visibility = ["//srp/mw/dtc:__subpackages__"],
)

cc_library(
    name = "dtc_database",
    deps = [
        "//core",
        "//communication-core:communication_core"
        ],
    srcs = ["database/dtc_database.cpp","database/dtc_database_element.cpp"],
    hdrs = ["database/dtc_database.hpp","database/dtc_database_element.hpp"],
    visibility = ["//srp/mw/dtc:__subpackages__"],
)

cc_library(
    name = "dtc_app",
    deps = [
        ":dtc_msg_factory",
        ":dtc_header",
        ":dtc_database",
        "//communication-core:communication_core"
        ],
    srcs = ["service/dtcService.cpp"],
    hdrs = ["service/dtcService.hpp","service/IDtcService.hpp"],
    visibility = ["//srp/mw/dtc:__subpackages__"]
)
cc_binary(
    name = "dtc_service",
    deps = [":dtc_app"],
    srcs = ["mainDtcService.cpp"],
    data = ["//mw/diag/dtc:config.json"]
)
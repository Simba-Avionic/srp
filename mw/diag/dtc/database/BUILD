cc_library(
    name = "dtc_db_element",
    deps = [
        "//communication-core/network-data:network_data_structure"
        ],
    srcs = ["dtc_database_element.cpp"],
    hdrs = ["dtc_database_element.hpp"],
    visibility =  ["//visibility:public"]
)
cc_library(
    name = "dtc_db",
    deps = [
        "//core",
        "//communication-core/network-data:network_data_structure",
        ":dtc_db_element"
        ],
    srcs = ["dtc_database.cpp"],
    hdrs = ["dtc_database.hpp"],
    visibility =  ["//visibility:public"]
)

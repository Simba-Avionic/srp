cc_library(
    name = "someip_lib",
    srcs = [
        "event_proxy.cc",
        "method_proxy.cc",
        "service_skeleton.cc",
        "someip_handler.cc",
    ],
    hdrs = [
        "event_proxy.h",
        "event_skeleton.h",
        "method_proxy.h",
        "service_skeleton.h",
        "someip_handler.h",
        "service_proxy.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com:com_error_domain",
        "//ara/com/someip:someip_data",
        "//ara/com/someip/bindings:bindings_lib",
        "//ara/core:Result",
        "//ara/core:instance_specifier",
        "//ara/core:model_db_lib",
    ],
)

cc_library(
    name = "someip_data",
    srcs = [
        "someip_frame.cc",
    ],
    hdrs = [
        "EventEntry.h",
        "HeaderStructure.h",
        "ServiceEntry.h",
        "message_code.h",
        "message_type.h",
        "network_endpoint.h",
        "someip_frame.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//ara/com:com_error_domain",
        "//ara/core:Result",
        "//ara/core:instance_specifier",
    ],
)

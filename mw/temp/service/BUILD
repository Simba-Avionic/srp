# cc_binary(
#     name = "temp_service_mw",
#     srcs = [
#         "main.cc",
#     ],
#     visibility = [
#         "//deployment:__subpackages__",
#         "//mw/temp:__subpackages__"
#     ],
#     deps = [
#         "//mw/temp/service:temp_service_lib",
#     ],
# )

# temp_drivers_deps = select({
#     "//:sim": ["//core/temp:SIM_temp_driver"],
#     "//conditions:default": ["//core/temp:temp_driver"],
# })

# cc_library(
#     name = "temp_service_lib",
#     deps = [
#         "//communication-core/network-data:network_data_structure",
#         "//communication-core/network-data:network_data_type",
#         "//communication-core/sockets:socket_interface",
#         "//communication-core/sockets:socket_ipc",
#         "//mw/temp/subscribe_msg",
#         "//mw/temp/temp_reading_msg:temp_reading_msg_factory",
#         "//core/json:simba_json",
#         "//core/common:condition_lib",
#         "//diag/jobs/skeleton:did_job_lib",
#         "@srp_platform//ara/exec:adaptive_application_lib",
#         "//deployment/mw/temp:ara",
#     ] + temp_drivers_deps,
#     srcs = [
#         "temp_service.cpp",
#         ],
#     hdrs = [
#         "temp_service.h",
#         "temp_mw_did.h",
#     ],
# )
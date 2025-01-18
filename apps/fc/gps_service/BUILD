# cc_library(
#     name = "gps_app_lib",
#     deps = [
#         "@srp_platform//ara/exec:adaptive_application_lib",
#         "//core/common:condition_lib",
#         "//deployment/apps/fc/gps_app:someip_lib",
#         "//deployment/apps/fc/gps_app:ara",
#         "//core/gps:gps_data_parser",
#         "//core/uart:uart_driver",
#     ],
#     srcs = [
#         "gps_app.cpp",
#     ],
#     hdrs = [
#         "gps_app.hpp",
#     ],
#     visibility = ["//apps/fc/gps_service:__subpackages__",],
# )

# cc_binary(
#     name = "gps_service",
#     srcs = [
#         "main.cc",
#     ],
#     visibility = [
#         "//deployment:__subpackages__",
#     ],
#     deps = [
#         "//apps/fc/gps_service:gps_app_lib",
#     ],
# )

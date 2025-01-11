# load("//deployment/tools/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")


# filegroup(
#     name = "instance",
#     srcs = [
#         "app_config.json",
#         "//deployment/system_definition/someip/fc/gps_service:service_someipy",
#     ],
#     visibility = [
#         "//apps/fc/gps_service:__subpackages__",
#         "//deployment/apps/fc/gps_service:__subpackages__",
#     ],
# )

# ara_someip_lib(
#     name = "someip_lib",
#     model_src = ["//deployment/apps/fc/gps_app:instance"],
#     visibility = ["//apps/fc/gps_service:__subpackages__"],
# )

# ara_runtime_lib(
#     name = "ara",
#     model_src = ["//deployment/apps/fc/gps_app:instance"],
#     visibility = ["//apps/fc/gps_service:__subpackages__"],
# )

# adaptive_application(
#     name = "GPSApp",
#     bin = "//apps/fc/gps_service",
#     model_src = ["//deployment/apps/fc/gps_app:instance"],
#     visibility = ["//deployment/cpu/fc:__subpackages__"],
# )


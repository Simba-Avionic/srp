# load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")


# filegroup(
#     name = "instance",
#     srcs = [
#         "app_config.json",
#         "//deployment/system_definition/someip/fc/recovery_service:service_someipy",
#         "//deployment/system_definition/diag/jobs/fc/recovery_service:recovery_job",
#     ],
#     visibility = [
#         "//apps/fc/recovery_service:__subpackages__",
#         "//deployment/apps/fc/recovery_service:__subpackages__",
#     ],
# )

# ara_someip_lib(
#     name = "someip_lib",
#     model_src = ["//deployment/apps/fc/recovery_service:instance"],
#     visibility = ["//apps/fc/recovery_service:__subpackages__"],
# )

# ara_runtime_lib(
#     name = "ara",
#     model_src = ["//deployment/apps/fc/recovery_service:instance"],
#     visibility = ["//apps/fc/recovery_service:__subpackages__"],
# )

# adaptive_application(
#     name = "RecoveryService",
#     bin = "//apps/fc/recovery_service:RecoveryService",
#     model_src = ["//deployment/apps/fc/recovery_service:instance"],
#     etcs = ["config.json"],
#     visibility = [
#         "//apps/fc/recovery_service:__subpackages__",
#         "//deployment/cpu/fc:__subpackages__",
#         ],
# )


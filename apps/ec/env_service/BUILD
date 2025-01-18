# load("@srp_platform//tools/model_generator/ara:adaptive_application.bzl", "adaptive_application", "ara_runtime_lib", "ara_someip_lib")

# ara_runtime_lib(
#     name = "ara",
#     model_src = ["//deployment/apps/env_service:instance"],
#     visibility = ["//apps/ec/env_service:__subpackages__"],
# )

# ara_someip_lib(
#     name = "someip_lib",
#     model_src = ["//deployment/apps/env_service:instance"],
#     visibility = ["//apps/ec/env_service:__subpackages__"],
# )

# cc_binary(
#     name = "env_service",
#     srcs = [
#         "main.cpp",
#         "env_service.cpp",
#         "env_service.hpp"
#     ],
#     visibility = [
#         "//deployment/apps:__subpackages__"
#         ],
#     deps = [
#         "@srp_platform//ara/exec:adaptive_application_lib",
#         "//core/common:condition_lib",
#         "@srp_platform//ara/diag:uds_lib",
#         "//mw/temp/controller:temp_controller_mw",
#         "//deployment/apps/env_service:ara",
#         "//deployment/apps/env_service:someip_lib",
#     ],
# )

cc_binary(
    name = "example_service",
    srcs = [
        "main.cpp",
        "example_app.cpp",
        "example_app.hpp",
        "KalmanFilter.cpp", "KalmanFilter.h", "KalmanFilterR7.h","KalmanFilterR7.cpp"
    ],
    visibility = [
        "//deployment/apps:__subpackages__"
        ],
    deps = [
        "@srp_platform//ara/exec:adaptive_application_lib",
        "//core/common:condition_lib",
        "//deployment/apps/example_service:ara",
        "@eigen", "//apps/ec/example_service/csvReader:csvReader"
    ],
)
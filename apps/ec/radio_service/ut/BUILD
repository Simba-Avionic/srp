# cc_test(
#     name = "radio_app_test",
#     srcs = [
#         "radio_app_test.cc"
#     ],
#     visibility = ["//visibility:public"],
#     deps = [
#         "@com_google_googletest//:gtest_main",
#         "//apps/fc/radio_service:radio_app_lib",
#         "//core/uart:mock_uart",
#         "//core/timestamp:mock_timestamp_controller",
#     ],
# )
# TODO(matikrajek42@gmail.com) fix UT

# cc_test(
#     name = "event_data_test",
#     srcs = [
#         "event_data_test.cc",
#     ],
#     visibility = ["//visibility:public"],
#     deps = [
#         "@com_google_googletest//:gtest_main",
#         "//apps/fc/radio_service:radio_app_lib",
#     ],
# )

cc_test(
    name = "gs_hb_guider_test",
    srcs = ["test_gs_hb_guider.cpp"],
    deps = [
        "//apps/ec/radio_service:gs_hb_guider",
        "@com_google_googletest//:gtest_main",
    ],
)

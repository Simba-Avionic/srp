cc_test(
    name = "rocket_state_transitions_test",
    srcs = ["test_transitions.cpp"],
    deps = [
        "//core/rocket_machine_state:rocket_state_base",
        "@com_google_googletest//:gtest_main",
    ],
)
cc_test(
    name = "rocket_state_locking_test",
    srcs = ["test_state_locking.cpp"],
    deps = [
        "//core/rocket_machine_state:rocket_state_base",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
  name = "rocket_state_audit_bugs_test",
  srcs = ["test_audit_bugs.cpp"],
  tags = ["audit_bugs"],
  deps = [
        "//core/rocket_machine_state:rocket_state_base",
        "@com_google_googletest//:gtest_main",
  ],
)

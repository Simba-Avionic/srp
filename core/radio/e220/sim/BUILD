load("@rules_python//python:defs.bzl", "py_library", "py_test")

# Atrapa modemu Ebyte E220 (testonly w praktyce: uzywana tylko przez testy/E2E); opis: README.md.
py_library(
    name = "e220_sim_lib",
    srcs = ["e220_sim.py"],
    imports = ["."],
    visibility = ["//visibility:public"],
)

py_test(
    name = "e220_sim_test",
    srcs = ["e220_sim_test.py"],
    main = "e220_sim_test.py",
    deps = [
        ":e220_sim_lib",
        "@srp_platform//third_party/python:pytest",
    ],
)

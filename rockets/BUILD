package(default_visibility = ["//visibility:public"])

# Rakieta: --config=r8 / --config=rurku (.bazelrc -> --define rocket=r8|rurku).
# Bez flagi zadne is_* nie pasuje i rocket.bzl bierze R8.
config_setting(
    name = "is_r8",
    define_values = {"rocket": "r8"},
)

config_setting(
    name = "is_rurku",
    define_values = {"rocket": "rurku"},
)

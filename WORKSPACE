workspace(name="srp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Toolchainy (BBB armv7, RPi aarch64 musl) rejestruje i pobiera srp_platform: install() ponizej.

load("//bazel/libs:third_party_repositories.bzl", "include_srp_platform", "include_srp_mavlink",
     "include_gtest_mock", "include_simdjson", "include_json", "include_simulation_data")

include_srp_platform("20261128-112043")

load("@srp_platform//:download.bzl", "download")
download()

load("@srp_platform//:install.bzl", "install")
install()

load("@srp_platform//:install_python.bzl", "install_python")
install_python()

load("@srp_platform//:pip_install.bzl", "pip_install")
pip_install()



include_srp_mavlink("0.14.0")
include_gtest_mock()
include_json("3.11.3")
include_simdjson("4.4.2", "284f1c5936e7eede83ce53ed4b03bcb09af02e3ddfec0aa465df109a8f6d3e87")
include_simulation_data()
##### PYTHON
# Python toolchain, rules_python and pip deps come from srp_platform (download(), install(),
# install_python(), pip_install()); srp has no pip packages of its own.

http_archive(
    name = "doipclient",
    strip_prefix = "python-doipclient-1.1.1/doipclient",
    build_file = "//bazel/libs:doipclient.BUILD",
    urls = ["https://github.com/jacobschaer/python-doipclient/archive/refs/tags/v1.1.1.zip"],
    type = "zip",
)

workspace(name="srp")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


srp_platform_version = "0.0.6SQF"
http_archive(
    name = "srp_platform",
    strip_prefix = "srp_platform-"+srp_platform_version,
    urls = ["https://github.com/SRP-Platform/srp_platform/archive/refs/tags/"+srp_platform_version+".zip"],
    type = "zip",
)

load("@srp_platform//:download.bzl", "download")
download()

load("@srp_platform//:install.bzl", "install")
install()

load("@srp_platform//:install_python.bzl", "install_python")
install_python()

load("@srp_platform//:pip_install.bzl", "pip_install")
pip_install()

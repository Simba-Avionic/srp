workspace(name="srp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//bazel/toolchain:toolchain.bzl", "register_all_toolchains")
register_all_toolchains()

load("//third_party:deps.bzl", "deps")
deps()

load("//bazel/libs:third_party_repositories.bzl", "include_srp_platform", "include_srp_mavlink", "include_gtest_mock", "include_json")
include_srp_platform("0.0.7-beta-patch-4", "Simba-Avionic/srp_platform", "e33efcd32410dc6d6cfe070a5a6fd60798fd7bf7d8dcbe3649fb9870e6ae070d")
load("@srp_platform//:download.bzl", "download")
download()

load("@srp_platform//:install.bzl", "install")
install()

load("@srp_platform//:install_python.bzl", "install_python")
install_python()

load("@srp_platform//:pip_install.bzl", "pip_install")
pip_install()



include_srp_mavlink("0.9.0")
include_gtest_mock()
include_json("3.11.3")

##### PYTHON

http_archive(
    name = "doipclient",
    strip_prefix = "python-doipclient-1.1.1/doipclient",
    build_file = "//bazel/libs:doipclient.BUILD",
    urls = ["https://github.com/jacobschaer/python-doipclient/archive/refs/tags/v1.1.1.zip"],
    type = "zip",
)

http_archive(
    name = "rules_python",
    sha256 = "9d04041ac92a0985e344235f5d946f71ac543f1b1565f2cdbc9a2aaee8adf55b",
    strip_prefix = "rules_python-0.26.0",
    url = "https://github.com/bazelbuild/rules_python/releases/download/0.26.0/rules_python-0.26.0.tar.gz",
)
load("@rules_python//python:repositories.bzl", "py_repositories")

py_repositories()

load("@rules_python//python:repositories.bzl", "python_register_toolchains")

python_register_toolchains(
    name = "python_3_11",
    # Available versions are listed in @rules_python//python:versions.bzl.
    # We recommend using the same version your team is already standardized on.
    python_version = "3.11",
)

load("@python_3_11//:defs.bzl", "interpreter")

load("@rules_python//python:pip.bzl", "pip_parse")

pip_parse(
    name = "pip_deps",
    requirements_lock = "//third_party/python:requirements.txt",
)

load("@pip_deps//:requirements.bzl", "install_deps")

install_deps()

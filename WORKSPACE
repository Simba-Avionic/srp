workspace(name="srp")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


load("//bazel/toolchain:toolchain.bzl", "register_all_toolchains")
register_all_toolchains()

load("//third_party:deps.bzl", "deps")
deps()

load("//bazel/libs:third_party_repositories.bzl", "include_spdlog", "include_srp_platform")
include_spdlog()
include_srp_platform("0.0.6SQ")
load("@srp_platform//:download.bzl", "download")
download()

load("@srp_platform//:install.bzl", "install")
install()

load("@srp_platform//:install_python.bzl", "install_python")
install_python()

load("@srp_platform//:pip_install.bzl", "pip_install")
pip_install()

http_archive(
    name = "com_github_nelhage_rules_boost",
    url = "https://github.com/nelhage/rules_boost/archive/be96ab4146e5b1d1db52a69482845dab883c4781.tar.gz",
    strip_prefix = "rules_boost-be96ab4146e5b1d1db52a69482845dab883c4781",
    integrity = "sha256-C7k9y8AqUBOzpQr3FkXTqiEyyaAaMhhK3z3NXUZUtYY=",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

http_archive(
    name = "com_google_benchmark",
    sha256 = "bdefa4b03c32d1a27bd50e37ca466d8127c1688d834800c38f3c587a396188ee",
    strip_prefix = "benchmark-1.5.3",
    urls = ["https://github.com/google/benchmark/archive/v1.5.3.zip"],
)

http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-5ab508a01f9eb089207ee87fd547d290da39d015",
    sha256 = "755f9a39bc7205f5a0c428e920ddad092c33c8a1b46997def3f1d4a82aded6e1",
    urls = ["https://github.com/google/googletest/archive/5ab508a01f9eb089207ee87fd547d290da39d015.zip"],
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
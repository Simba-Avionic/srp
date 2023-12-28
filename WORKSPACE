workspace(name="srp")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

load("//third_party:deps.bzl", "deps")
deps()

load("//bazel/toolchain:toolchain.bzl", "register_all_toolchains")
register_all_toolchains()

load("//bazel/libs:third_party_repositories.bzl", "include_spdlog")
include_spdlog()

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

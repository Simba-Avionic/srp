load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

def include_spdlog():

    http_archive(
        name = "com_github_fmtlib_fmt",
        sha256 = "3c2e73019178ad72b0614a3124f25de454b9ca3a1afe81d5447b8d3cbdb6d322",
        strip_prefix = "fmt-10.1.1",
        urls = ["https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.zip"],
        build_file = "//bazel/libs:fmtlib.BUILD",
    )

    http_archive(
        name = "com_github_gabime_spdlog",
        build_file = "//bazel/libs:spdlog.BUILD",
        sha256 = "6174bf8885287422a6c6a0312eb8a30e8d22bcfcee7c48a6d02d1835d7769232",
        strip_prefix = "spdlog-1.12.0",
        urls = ["https://github.com/gabime/spdlog/archive/refs/tags/v1.12.0.zip"],
    )

    http_archive(
        name = "com_json",
        strip_prefix = "json-3.11.3",
        urls = ["https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip"],
        sha256 = "04022b05d806eb5ff73023c280b68697d12b93e1b7267a0b22a1a39ec7578069"
    )
    http_archive(
        name = "untar",
        strip_prefix = "untar-main",
        build_file = "//bazel/libs:untar.BUILD",
        urls = ["https://codeload.github.com/AlexisTM/untar/zip/refs/heads/main"],
        type = "zip",
        sha256 = "5b69c01e44ca184bf36d7fa4a19b7bb50d8014a256456e74177f221610865028",
    )

    http_archive(
        name = "doipclient",
        strip_prefix = "python-doipclient-1.1.1/doipclient",
        build_file = "//bazel/libs:doipclient.BUILD",
        urls = ["https://github.com/jacobschaer/python-doipclient/archive/refs/tags/v1.1.1.zip"],
        type = "zip",
    )

    
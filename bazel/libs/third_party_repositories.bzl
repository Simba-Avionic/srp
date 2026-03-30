load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

def include_json(ver):
    http_archive(
        name = "com_json",
        strip_prefix = "json-"+ver,
        urls = ["https://github.com/nlohmann/json/archive/refs/tags/v"+ver+".zip"],
        sha256 = "04022b05d806eb5ff73023c280b68697d12b93e1b7267a0b22a1a39ec7578069"
    )


def include_simdjson(ver, sha256):
    http_archive(
        name = "simdjson",
        urls = ["https://github.com/simdjson/simdjson/releases/download/v" + ver + "/singleheader.zip"],
        type = "zip",
        build_file = "//bazel/libs:simdjson.BUILD",
        sha256 = sha256,
    )

def include_srp_platform(ver, source):
        http_archive(
        name = "srp_platform",
        strip_prefix = "srp_platform-"+ver,
        urls = ["https://github.com/"+source+"/archive/refs/tags/"+ver+".zip"],
        type = "zip",
    )
    
def include_srp_mavlink(ver):
        http_archive(
        name = "srp_mavlink",
        strip_prefix = "mavlink_c",
        urls = ["https://github.com/Simba-Avionic/simba_mavlink/releases/download/v"+ver+"/simba_mavlink.zip"],
        type = "zip",
        build_file = "//bazel/libs:simba_mavlink.BUILD",
    )
def include_gtest_mock():
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


def include_simulation_data():
    http_archive(
        name = "simulation_data_apogee",
        url = "https://github.com/Simba-Avionic/srp-common/releases/download/v1.0/data.zip",
        integrity = "sha256-1vPb5VyAvPpXCEAbIfftT7OJgjA9yYpdkZSrbALR3Jw=", 
        build_file = "//bazel/libs:apogee_simulation_data.BUILD",
    )
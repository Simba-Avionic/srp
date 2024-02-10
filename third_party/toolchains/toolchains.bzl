load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchains():
    if "arm-cortex_a8-linux-gnueabihf" not in native.existing_rules():
        http_archive(
            name = "arm-cortex_a8-linux-gnueabihf",
            build_file = Label("//third_party/toolchains:arm-cortex_a8-linux-gnueabihf.BUILD"),
            url = "https://github.com/Simba-Avionic/fc_yocto/releases/download/v8.0/arm-cortex_a8-linux-gnueabihf.tar.gz",
            sha256 = "f2b74022f80ed02253b302e993907b57b9d13107be55d08bdaaae637a6ebf749",
        )

    if "arm-cortex_a8-linux-gnueabihf-sysroot" not in native.existing_rules():
        http_archive(
            name = "arm-cortex_a8-linux-gnueabihf-sysroot",
            build_file = Label("//third_party/toolchains:arm-cortex_a8-linux-gnueabihf-sysroot.BUILD"),
            url = "https://github.com/Simba-Avionic/fc_yocto/releases/download/v8.0/beaglebone.tar.gz",
            sha256 = "324e5db4daea321fba5fc283a1375340570e3857ee70395ceba12e67c299cee5",
        )

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def toolchains():
    if "arm-cortex_a8-linux-gnueabihf" not in native.existing_rules():
        http_archive(
            name = "arm-cortex_a8-linux-gnueabihf",
            build_file = Label("//third_party/toolchains:arm-cortex_a8-linux-gnueabihf.BUILD"),
            url = "https://github.com/Simba-Avionic/fc_yocto/releases/download/v0.1/arm-cortex_a8-linux-gnueabihf.tar.gz",
            sha256 = "c52c2830e5aaf83c608d870f199b72516f07bad33c8148622d8f03ac539500bb",
        )

    if "arm-cortex_a8-linux-gnueabihf-sysroot" not in native.existing_rules():
        http_archive(
            name = "arm-cortex_a8-linux-gnueabihf-sysroot",
            build_file = Label("//third_party/toolchains:arm-cortex_a8-linux-gnueabihf-sysroot.BUILD"),
            url = "https://github.com/Simba-Avionic/fc_yocto/releases/download/v0.1/beaglebone.tar.gz",
            sha256 = "f7ba2eb93e26f6bff99ddd640166c9a023443ebcf15893380a06fbcf926720bb",
        )

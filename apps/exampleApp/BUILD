cc_binary(
    name = "exampleApp",
    srcs = [
        "exampleApp.cc",
        "exampleApp.h",
	    "main.cc",
        
    ],

    visibility = ["//deployment/apps/example:__subpackages__"],
    deps = [
        "//core/application:simba_application",
    ],
)
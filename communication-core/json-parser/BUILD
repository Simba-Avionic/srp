cc_library(
    name = "JsonParser",
    # srcs = ["json_parser.cc"],
    hdrs = [
        # "Ijson_parser.h",
        "database_json_parser.h",
        # "json_parser.h",
    ],
    visibility = [
        "//apps:__subpackages__",
        "//mw:__subpackages__",
        "//communication-core:__subpackages__",
    ],
    deps = [
        "//communication-core/database:DatabaseLib",
        "@com_json//:json",
    ],
)

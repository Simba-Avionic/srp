
py_binary(
    name = "main",
    srcs = ["main.py"],
    deps = [":libs"],
)


# py_test(
#     name = "crc_generator_test",
#     deps = [
#         ":libs"
#         ],
#     srcs = ["crc_generator_test.py"],
#     imports = ["unittests"],
# )

# TODO: FIX this test
# py_test(
#     name = "file_reader_test",
#     deps = [
#         ":libs"
#         ],
#     srcs = ["file_reader_test.py"],
#     imports = ["unittests"],
#     data=["test_tar.tar"]
# )

py_library(
    name = "libs",
    srcs = glob(["*.py"]),
    visibility = ["//deployment/tools/ota_sender_script:__subpackages__"],
)
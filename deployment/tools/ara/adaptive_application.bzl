load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")
load("//deployment/bazel:connect_tar.bzl", "connect_tar")

def _impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_file(ctx.attr.out_name)
    args = [out.path] + [f.path for f in ctx.files.src]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )
    return [DefaultInfo(files = depset([out]))]

rename = rule(
    implementation = _impl,
    attrs = {
        "src": attr.label_list(mandatory = False, allow_files = True),
        "out_name": attr.string(),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/bazel:rename_sh"),
        ),
    },
)

def ara_json2config_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_directory("etc")
    args = [out.path, ctx.files.src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )

    return [DefaultInfo(files = depset(direct = [out]))]

ara_json2config = rule(
    implementation = ara_json2config_impl,
    attrs = {
        "src": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/ara:json2config"),
        ),
    },
)

def ara_json2runtime_env_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_directory("ara/core")
    args = [out.path, ctx.files.src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )

    return [DefaultInfo(files = depset([out]))]

ara_json2runtime_env = rule(
    implementation = ara_json2runtime_env_impl,
    attrs = {
        "src": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/ara:json2runtime_env"),
        ),
    },
)

def ara_json2someip_lib_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_directory("someip_lib.h")
    args = [out.path, ctx.files.src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )

    return [DefaultInfo(files = depset([out]))]

ara_json2someip_lib = rule(
    implementation = ara_json2someip_lib_impl,
    attrs = {
        "src": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/ara:json2someip_lib"),
        ),
    },
)

def ara_someip_lib(name, model_src, visibility = []):
    ara_json2someip_lib(
        name = "someip_src",
        src = model_src,
        visibility = ["//visibility:private"],
    )

    native.cc_library(
        name = name,
        deps = ["//ara/com:com_error_domain", "//ara/core:Result", "//ara/com/someip:someip_lib"],
        srcs = [":someip_src"],
        includes = ["./someip_lib.h"],
        visibility = visibility,
    )

def ara_runtime_lib(name, model_src, visibility = []):
    ara_json2runtime_env(
        name = "runtime_src",
        src = model_src,
        visibility = ["//visibility:private"],
    )

    native.cc_library(
        name = name,
        deps = ["//ara/core:ara_initialization_lib", "//ara/log", "//ara/com"],
        srcs = [":runtime_src"],
        includes = ["."],
        visibility = visibility,
    )

def adaptive_application(name, model_src, bin, etcs = [], visibility = []):
    ara_json2config(
        name = "config",
        src = model_src,
        visibility = ["//visibility:public"],
    )
    rename(
        name = "out_bin",
        out_name = name,
        src = [bin],
        visibility = ["//visibility:private"],
    )
    pkg_tar(
        name = "config_files",
        package_dir = "opt/" + name,
        srcs = [":config"] + etcs,
        visibility = ["//visibility:private"],
    )
    if len(etcs) != 0:
        pkg_tar(
            name = "config_files_other",
            package_dir = "opt/" + name + "/etc",
            srcs = etcs,
            visibility = ["//visibility:private"],
        )

    pkg_tar(
        name = "bin-pkg",
        package_dir = "opt/" + name + "/bin",
        srcs = [
            ":out_bin",
        ],
        visibility = ["//visibility:private"],
    )
    if len(etcs) != 0:
        connect_tar(
            name = name,
            srcs = [
                ":bin-pkg",
                ":config_files",
                ":config_files_other",
            ],
            visibility = visibility,
        )
    else:
        connect_tar(
            name = name,
            srcs = [
                ":bin-pkg",
                ":config_files",
            ],
            visibility = visibility,
        )

def ara_json2sm_lib_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_directory("smcomponents.h")
    out2 = ctx.actions.declare_file("initialization.cc")
    args = [out.path, out2.path, ctx.files.src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out, out2],
        arguments = args,
        executable = ctx.executable.tool,
    )

    return [DefaultInfo(files = depset([out, out2]))]

ara_json2sm_code = rule(
    implementation = ara_json2sm_lib_impl,
    attrs = {
        "src": attr.label(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/ara:json2sm_code"),
        ),
    },
)

def ara_json2sm_config_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_file("state_config.json")
    args = [out.path, ctx.files.src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )

    return [DefaultInfo(files = depset([out]))]

ara_json2sm_config = rule(
    implementation = ara_json2sm_config_impl,
    attrs = {
        "src": attr.label(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/ara:json2sm_config"),
        ),
    },
)

def ara_json2sm_lib(name, src, visibility = []):
    ara_json2sm_code(
        name = name + ".files",
        src = src,
        visibility = ["//visibility:private"],
    )

    native.cc_library(
        name = name,
        visibility = visibility,
        srcs = [":" + name + ".files"],
        hdrs = ["//ara/exec/sm"],
        includes = ["./smcomponents.h", "."],
        deps = [
            "//ara/exec/sm:sm_interfaces",
            "//ara/exec/sm:state_meager_lib",
        ],
    )

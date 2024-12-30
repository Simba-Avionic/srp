
def ara_json2sm_lib_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_directory("smcomponents.h")
    args = [out.path, ctx.files.src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )

    return [DefaultInfo(files = depset(direct = [out]))]

ara_json2sm_lib = rule(
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

    return [DefaultInfo(files = depset(direct = [out]))]

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
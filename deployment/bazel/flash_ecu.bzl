
def _impl(ctx):
    # The list of arguments we pass to the script.
    user = ctx.attr.user_name+"@"+ctx.attr.ip
    out = ctx.actions.declare_file("log.txt")
    args = [out.path, user, ctx.files.file[0].path, ctx.files.file[0].basename.replace(".tar","")]
    
    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.file,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )
    return [DefaultInfo(files = depset([out]))]



flash_ecu = rule(
    implementation = _impl,
    attrs = {
        "file": attr.label_list(mandatory = False, allow_files = True),
        "ip": attr.string(),
        "user_name": attr.string(),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/bazel:flash_sh"),
        ),
    },
)
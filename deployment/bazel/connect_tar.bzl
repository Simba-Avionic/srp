
def _impl_connect(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_file(ctx.attr.name+".tar")
    args = [out.path] + [f.path for f in ctx.files.srcs]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.srcs,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )
    return [DefaultInfo(files = depset([out]))]

connect_tar = rule(
    implementation = _impl_connect,
    attrs = {
        "srcs": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/bazel:connect_tar_sh"),
        ),
    },
)
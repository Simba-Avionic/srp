
def _impl_diag(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_file("app_diag.json")
    args = [out.path, ctx.files.config_src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.config_src + ctx.files.includes,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )
    return [DefaultInfo(files = depset([out]))]

bs_diag_gen = rule(
    implementation = _impl_diag,
    attrs = {
        "config_src": attr.label_list(mandatory = False, allow_files = True),
        "includes": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/configs/diag:app_diag_gen"),
        ),
    },
)


def _impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_file("logger.json")
    args = [out.path, ctx.files.config_src[0].path]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.config_src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )
    return [DefaultInfo(files = depset([out]))]

logger_gen = rule(
    implementation = _impl,
    attrs = {
        "config_src": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/configs/logger_generator:logger"),
        ),
    },
)

def _someip_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_file("app_someip.json")
    args = [out.path, ctx.files.config_src[0].path]
    for i in ctx.files.includes:
        args.append(i.path)

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.config_src + ctx.files.includes,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )
    return [DefaultInfo(files = depset([out]))]

bs_someip_gen = rule(
    implementation = _someip_impl,
    attrs = {
        "config_src": attr.label_list(mandatory = False, allow_files = True),
        "includes": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/configs/local_app_generator:generator"),
        ),
    },
)

def _app_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_file("srp_app.json")
    args = [out.path, ctx.files.config_src[0].path, "/opt/"+ctx.attr.component_name+"/bin/"+ctx.attr.component_name]

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.config_src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )
    return [DefaultInfo(files = depset([out]))]

bs_app_gen = rule(
    implementation = _app_impl,
    attrs = {
        "config_src": attr.label_list(mandatory = False, allow_files = True),
        "component_name": attr.string(),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/configs/local_app_generator:app_generator"),
        ),
    },
)

def config_gen(name, component_name, config_src, includes = [], includes_diag=[], visibility = []):
    logger_gen(
        name = "logger",
        config_src = config_src,
    )
    bs_someip_gen(
        name = "someip_config",
        config_src = config_src,
        includes = includes,
    )
    bs_app_gen(
        name = "app_config_r",
        config_src = config_src,
        component_name = component_name,
    )
    bs_diag_gen(
        name = "diag",
        config_src = config_src,
        includes = includes_diag,
    )
    native.filegroup(
        name = name,
        visibility = visibility,
        srcs = [
            ":logger",
            ":someip_config",
            ":app_config_r",
            ":diag",
        ],
    )

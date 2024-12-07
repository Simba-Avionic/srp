def ara_data_structure_generator_impl(ctx):
    # The list of arguments we pass to the script.
    out = ctx.actions.declare_directory("data_structure.h")
    args = [out.path]
    for arg in ctx.files.src:
        args.append(arg.path)

    # Action to call the script.
    ctx.actions.run(
        inputs = ctx.files.src,
        outputs = [out],
        arguments = args,
        executable = ctx.executable.tool,
    )

    return [DefaultInfo(files = depset([out]))]

ara_data_structure_generator = rule(
    implementation = ara_data_structure_generator_impl,
    attrs = {
        "src": attr.label_list(mandatory = False, allow_files = True),
        "tool": attr.label(
            executable = True,
            cfg = "exec",
            allow_files = True,
            default = Label("//deployment/tools/ara:data_structure_gen"),
        ),
    },
)

def data_structure_generator(name, model = [], visibility = []):
    ara_data_structure_generator(
        name = "data_structure_src",
        src = model,
        visibility = ["//visibility:private"],
    )
    native.cc_library(
        name = name,
        srcs = [":data_structure_src"],
        deps = ["//ara/com:com_error_domain", "//ara/core:Result"],
        includes = ["./data_structure.h"],
        visibility = visibility,
    )

def _flash_script(ctx):
    user = ctx.attr.user_name + "@" + ctx.attr.ip
    content = """ 
#!/bin/sh
################################################################################
#
#   Copyright (c) 2024 Bartosz Snieg.
#
################################################################################
#
now=`date '+%Y_%m_%d_%H_%M_%S'`
file_name=\""""+ctx.files.file[0].path[21:]+"""\"
echo "SRP 2.0 Flash tool [Start]"
echo "Flash tool: removing old sw"
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null """+user+""" "rm -rf /srp/opt/*"
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null """+user+""" "rm -rf /srp/platform/*"
echo "Flash tool: Uploading new sw"
scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $file_name \""""+user+""":/srp/update/new/"""+ctx.files.file[0].basename.replace(".tar","")+"""-$now.tar\"
echo "Flash tool: Unpacking a new software version"
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null """+user+""" "tar -xf /srp/update/new/"""+ctx.files.file[0].basename.replace(".tar","")+"""-$now.tar -C /srp"
echo "Flash tool: Cleaning"
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null """+user+""" " sleep 2 & rm -rf /srp/update/new/"""+ctx.files.file[0].basename.replace(".tar","")+"""-$now.tar"
echo "Flash tool: ECU reboot"
timeout 2 ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ConnectTimeout=2  """+user+""" " sleep 2 & reboot"
echo "SRP 2.0 Flash tool [Stop]"
"""
    return content

def _impl(ctx):
    # The list of arguments we pass to the script.
    # user = ctx.attr.user_name+"@"+ctx.attr.ip
    # out = ctx.actions.declare_file("log.txt")
    # args = [out.path, user, ctx.files.file[0].path, ctx.files.file[0].basename.replace(".tar","")]

    # # Action to call the script.
    # ctx.actions.run(
    #     inputs = ctx.files.file,
    #     outputs = [out],
    #     arguments = args,
    #     executable = ctx.executable.tool,
    # )
    out = ctx.actions.declare_file("flash_script.sh")
    ctx.actions.write(output = out, content = _flash_script(ctx))
    return [DefaultInfo(files = depset([out]))]

_flash_ecu = rule(
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

def flash_ecu(name, file = [], ip = "", user_name = "",tool=""):
    native.filegroup(name = name+".data",srcs=file);
    _flash_ecu(
        name = name + ".script",
        file = file,
        ip = ip,
        user_name = user_name,
    )
    native.sh_binary(
        name = name,
        srcs = [":" + name + ".script",],
        data = [":"+name+".data"],
        args = file
    )

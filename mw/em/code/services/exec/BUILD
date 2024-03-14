cc_library(
    name = "exec_service",
    srcs = [
        "exec_manager.cpp",
    ],
    hdrs = [
        "exec_manager.hpp",
    ],
    deps = [
        "//core/application:simba_application",
        "//communication-core/sockets:socket_ipc",
        "//core/logger:Logger",
        "//diag/exec/data:diag_exec_header",
        "//mw/em/code/services/em:em_config",
    ],
    visibility = [
        "//mw:__subpackages__",
    ],
)

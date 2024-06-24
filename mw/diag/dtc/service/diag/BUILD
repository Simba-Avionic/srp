cc_library(
    name = "uds_jobs",
    srcs = [
        "dtc_by_mask_job.cc",
        "dtc_list_job.cc",
        "dtc_snapshot_job.cc",
    ],
    hdrs = [
        "dtc_by_mask_job.h",
        "dtc_list_job.h",
        "dtc_snapshot_job.h",
    ],
    visibility = ["//mw/diag/dtc:__subpackages__"],
    deps = [
        "//mw/diag/dtc/service/lib:converter_lib",
        "//core/logger:Logger",
        "//diag/jobs/skeleton:other_job_lib",
        "//mw/diag/dtc/service:db_lib",
    ],
)

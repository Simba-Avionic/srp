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
        "//core/logger:Logger",
        "//diag/jobs/skeleton:other_job_lib",
    ],
)

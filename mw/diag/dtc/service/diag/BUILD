cc_library(
    name = "uds_jobs",
    srcs = [
        "clear_dtc_job.cc",
        "dtc_by_mask_job.cc",
        "dtc_list_job.cc",
        "dtc_snapshot_job.cc",
    ],
    hdrs = [
        "clear_dtc_job.h",
        "dtc_by_mask_job.h",
        "dtc_list_job.h",
        "dtc_snapshot_job.h",
    ],
    visibility = ["//mw/diag/dtc:__subpackages__"],
    deps = [
        "//core/logger:Logger",
        "//diag/jobs/skeleton:other_job_lib",
        "//mw/diag/dtc/service:db_lib",
        "//mw/diag/dtc/service/lib:converter_lib",
    ],
)

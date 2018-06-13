
utils_files = files(
    'list.c',
    'list.h',
)

libutils_virgl = static_library(
    'utils_virgl',
    [ utils_files ],
    c_args : vgl_flags,
    include_directories : [
        inc_common,
    ],
    link_with: [
    ],
)

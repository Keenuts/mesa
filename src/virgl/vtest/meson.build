vgl_vtest_files = files(
    'vtest_socket.c',
)

libvtest_virgl = static_library(
    'vtest_virgl',
    [ vgl_vtest_files ],
    c_args : c_vis_args,
    include_directories : [
        inc_common,
        inc_virgl,
    ]
)

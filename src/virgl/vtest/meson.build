vgl_vtest_files = files(
    'vtest_commands.c',
    'vtest_device.c',
    'vtest_objects.c',
    'vtest_socket.c',
)

libvtest_virgl = static_library(
    'vtest_virgl',
    [ vgl_vtest_files ],
    c_args : vgl_flags,
    include_directories : [
        inc_common,
        inc_virgl,
    ],
    link_with: [
        libgallium,
    ],
)

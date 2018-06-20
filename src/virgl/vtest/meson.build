vgl_codegen_vtest_objects = custom_target(
    'vtest_objects.[ch]',
    input : [
        vgl_codegen_objects_py,
        vk_api_xml,
        vgl_codegen_objects_config,
    ],
    output : ['vtest_objects.c', 'vtest_objects.h'],
    command : [
        prog_python2, '@INPUT0@', '--xml', '@INPUT1@',
            '--outdir', meson.current_build_dir(),
    ],
    depend_files : vgl_codegen_objects_py,
)

vgl_vtest_files = files(
    'vtest_socket.c',
    'vtest_device.c',
)

libvtest_virgl = static_library(
    'vtest_virgl',
    [ vgl_codegen_vtest_objects, vgl_vtest_files ],
    c_args : vgl_flags,
    include_directories : [
        inc_common,
        inc_virgl,
    ],
    link_with: [
        libgallium,
    ],
)

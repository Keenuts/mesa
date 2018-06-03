vgl_entrypoints_gen_py = files('tools/vgl_entrypoints_gen.py')
vgl_icd_gen_py = files('tools/vgl_icd_gen.py')

vgl_entrypoints = custom_target(
    'vgl_entrypoint.[ch]',
    input : [
        vgl_entrypoints_gen_py,
        vk_api_xml,
        files(
            'tools/templates/vgl_entrypoints_template.c',
            'tools/templates/vgl_entrypoints_template.h',
            'tools/implemented.json',
        )
    ],
    output : ['vgl_entrypoints.c', 'vgl_entrypoints.h'],
    command : [
        prog_python2, '@INPUT0@', '--xml', '@INPUT1@',
            '--outdir', meson.current_build_dir(),
    ],
    depend_files : vgl_entrypoints_gen_py,
)

vgl_icd = custom_target(
    'vgl_icd',
    input : vgl_icd_gen_py,
    output : 'vgl_icd.x86_64.json',
    command : [
        prog_python2, '@INPUT@',
            '--lib-path', join_paths(get_option('prefix'), get_option('libdir')),
            '--out', '@OUTPUT@',
    ],
    depend_files : vgl_icd_gen_py,
    install_dir : with_vulkan_icd_dir,
    install : true,
)

dev_icd = custom_target(
    'dev_icd',
    input : vgl_icd_gen_py,
    output : 'dev_icd.x86_64.json',
    command : [
        prog_python2, '@INPUT@',
            '--lib-path', meson.current_build_dir(),
            '--out', '@OUTPUT@',
    ],
    depend_files : vgl_icd_gen_py,
    install_dir : with_vulkan_icd_dir,
    install : true,
)

vgl_vulkan_files = files(
    'vgl_device.c',
)

vgl_flags = [
    c_vis_args,
    no_override_init_args,
    '-Wall',
    '-Wextra',
    '-Werror',
]

libvulkan_virgl = shared_library(
    'vulkan_virgl',
    [ vgl_entrypoints, vgl_vulkan_files ],
    include_directories : [
        inc_common,
        inc_virgl,
    ],
    link_with: [
        libmesa_util,
    ],
    c_args : vgl_flags,
    dependencies : [ ],
    link_args : [],
    install : true
)

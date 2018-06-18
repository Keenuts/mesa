# This target generated the entrypoitns files
# To add an entrypoint implementation, add en entry in
# the '../tools/implemented.json' file.
vgl_codegen_entrypoints = custom_target(
    'vgl_entrypoint.[ch]',
    input : [
        vgl_codegen_entrypoints_py,
        vk_api_xml,
        vgl_codegen_entrypoints_config,
        vgl_codegen_entrypoints_templates,
    ],
    output : ['vgl_entrypoints.c', 'vgl_entrypoints.h'],
    command : [
        prog_python2, '@INPUT0@', '--xml', '@INPUT1@',
            '--outdir', meson.current_build_dir(),
    ],
    depend_files : vgl_codegen_entrypoints_py,
)

vgl_vulkan_files = files(
    'icd.c',
    'vk_device.c',
    'vk_objects.c',
    'memory.c',
)


libvulkan_virgl = shared_library(
    'vulkan_virgl',
    [ vgl_codegen_entrypoints, vgl_vulkan_files ],
    c_args : vgl_flags,
    include_directories : [
        inc_common,
        inc_virgl,
    ],
    link_with: [
        libmesa_util,
        libvtest_virgl,
        libutils_virgl,
    ],
    dependencies : [ ],
    link_args : [],
    install : true
)

inc_virgl = include_directories('.')

# Files used to generate vulkan ICD's json files
vgl_codegen_icdfiles_py       = files('tools/codegen_icdfiles.py')

# Files used to generate object creations related functions
vgl_codegen_objects_py         = files('tools/codegen_objects.py')
vgl_codegen_objects_config     = files('tools/obj_creation.json')

# Files used to generate vulkan ICD's entrypoints
# To add an entrypoint implementation, add en entry in
# the 'tools/implemented.json' file.
vgl_codegen_entrypoints_py        = files('tools/codegen_entrypoints.py')
vgl_codegen_entrypoints_config    = files('tools/implemented.json')
vgl_codegen_entrypoints_templates = files(
   'tools/templates/vgl_entrypoints_template.c',
   'tools/templates/vgl_entrypoints_template.h',)

vgl_flags = [
    c_vis_args,
    no_override_init_args,
    '-Wall',
    '-Wextra',
    '-Werror',
]

# Creates the release ICD json file.
release_icd_json = custom_target(
    'release_icd_json',
    input : vgl_codegen_icdfiles_py,
    output : 'virglrenderer.x86_64.json',
    command : [
        prog_python2, '@INPUT@',
            '--lib-path', join_paths(get_option('prefix'), get_option('libdir')),
            '--out', '@OUTPUT@',
    ],
    depend_files : vgl_codegen_icdfiles_py,
    install_dir : with_vulkan_icd_dir,
    install : true,
)

debug_icd_json = custom_target(
    'debug_icd_json',
    input : vgl_codegen_icdfiles_py,
    output : 'virglrenderer_debug.x86_64.json',
    command : [
        prog_python2, '@INPUT@',
            '--lib-path', meson.current_build_dir(),
            '--out', '@OUTPUT@',
    ],
    depend_files : vgl_codegen_icdfiles_py,
    install_dir : with_vulkan_icd_dir,
    install : true,
)

subdir('common')
subdir('vtest')

if with_virgl_vk
    subdir('vulkan')
endif

inc_virgl = include_directories('.')

vgl_flags = [
    c_vis_args,
    no_override_init_args,
    '-Wall',
    '-Wextra',
    '-Werror',
]

subdir('util')
subdir('vtest')

if with_virgl_vk
    subdir('vulkan')
endif

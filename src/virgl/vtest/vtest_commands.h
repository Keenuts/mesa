#pragma once
#include <vulkan/vulkan.h>

struct vtest_command_pool_create_info {
   uint32_t device_handle;
   uint32_t flags;
   uint32_t queue_family_index;
};

struct vtest_command_buffer_allocate_info {
   uint32_t device_handle;
   uint32_t pool_handle;
   uint32_t level;
   uint32_t count;
};

struct vtest_command_record_info {
   uint32_t device_handle;
   uint32_t cmd_handle;
   uint32_t pipeline_handle;
   uint32_t pipeline_layout_handle;
   uint32_t dispatch_size[3];
   uint32_t descriptor_count;
};

int vtest_create_command_pool(int sock_fd,
                              const struct vtest_command_pool_create_info *info,
                              uint32_t *handle);

int vtest_allocate_command_buffers(int sock_fd,
                                   const struct vtest_command_buffer_allocate_info *info,
                                   uint32_t *handles);

int vtest_record_command(int sock_fd,
                         const struct vtest_command_record_info *info,
                         const uint32_t *descriptor_handles);

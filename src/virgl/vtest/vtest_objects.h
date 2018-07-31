#pragma once
#include <vulkan/vulkan.h>

/* Structures */

struct payload_create_descriptor_set_layout_intro {
   uint32_t handle;
   uint32_t flags;
   uint32_t bindingCount;
};

struct payload_create_descriptor_set_layout_pBindings {
   uint32_t binding;
   uint32_t descriptorType;
   uint32_t descriptorCount;
   uint32_t stageFlags;
};

struct payload_create_buffer_intro {
   uint32_t handle;
   uint32_t flags;
   uint64_t size;
   uint32_t usage;
   uint32_t sharingMode;
   uint32_t queueFamilyIndexCount;
};

struct payload_allocate_descriptor_sets_intro {
   uint32_t handle;
   uint32_t descriptorPool;
   uint32_t descriptorSetCount;
};

struct payload_create_shader_module_intro {
   uint32_t handle;
   uint32_t flags;
   uint32_t codeSize;
};

struct payload_create_descriptor_pool_intro {
   uint32_t handle;
   uint32_t flags;
   uint32_t maxSets;
   uint32_t poolSizeCount;
};

struct payload_create_descriptor_pool_pPoolSizes {
   uint32_t type;
   uint32_t descriptorCount;
};

struct payload_create_pipeline_layout_intro {
   uint32_t handle;
   uint32_t flags;
   uint32_t setLayoutCount;
   uint32_t pushConstantRangeCount;
};

struct payload_create_pipeline_layout_pPushConstantRanges {
   uint32_t stageFlags;
   uint32_t offset;
   uint32_t size;
};

struct payload_create_compute_pipelines_intro {
    uint32_t handle;
    uint32_t flags;
    uint32_t layout;
    uint32_t stage_flags;
    uint32_t stage_stage;
    uint32_t stage_module;
    uint32_t entrypoint_len;
};

struct payload_allocate_memory {
    uint32_t handle;
    uint32_t memory_index;
    uint64_t device_size;
};

struct payload_bind_buffer_memory {
   uint32_t device_handle;
   uint32_t buffer_handle;
   uint32_t memory_handle;
   uint64_t offset;
};

struct payload_write_descriptor_set {
   uint32_t device_handle;
   uint32_t dst_set;
   uint32_t dst_binding;
   uint32_t dst_array_element;
   uint32_t descriptor_type;
   uint32_t descriptor_count;
};

struct payload_write_descriptor_set_buffer {
   uint32_t buffer_handle;
   uint64_t offset;
   uint64_t range;
};

struct payload_create_fence {
   uint32_t device_handle;
   uint32_t flags;
};

struct payload_wait_for_fences {
   uint32_t device_handle;
   uint32_t fence_count;
   uint32_t wait_all;
   uint32_t timeout;
   /* uint32_t fence_handles[] */;
};

struct payload_queue_submit {
   uint32_t device_handle;
   uint32_t queue_handle;
   uint32_t fence_handle;
   uint32_t wait_count;
   uint32_t cmd_count;
   uint32_t signal_count;
   /* uint32_t wait_handles[]; */
   /* uint32_t cmd_handles[]; */
   /* uint32_t signal_handles[]; */
};

struct payload_destroy_object {
   uint32_t device_handle;
   uint32_t object_handle;
};

/* Functions */

int vtest_create_descriptor_set_layout(int sock_fd,
   uint32_t handle,
   const VkDescriptorSetLayoutCreateInfo *create_info,
   uint32_t  *output);

int vtest_create_buffer(int sock_fd,
   uint32_t handle,
   const VkBufferCreateInfo *create_info,
   uint32_t  *output);

int vtest_allocate_descriptor_sets(int sock_fd,
   uint32_t handle,
   uint32_t pool_handle,
   const VkDescriptorSetAllocateInfo *create_info,
   uint32_t *handles,
   uint32_t  *output);

int vtest_create_shader_module(int sock_fd,
   uint32_t handle,
   const VkShaderModuleCreateInfo *create_info,
   uint32_t  *output);

int vtest_create_descriptor_pool(int sock_fd,
   uint32_t handle,
   const VkDescriptorPoolCreateInfo *create_info,
   uint32_t  *output);

int vtest_create_pipeline_layout(int sock_fd,
    uint32_t handle,
    const VkPipelineLayoutCreateInfo *create_info,
    uint32_t *set_handles,
    uint32_t  *output);

int vtest_create_compute_pipelines(int sock_fd,
    uint32_t device_handle,
    const VkComputePipelineCreateInfo *info,
    uint32_t handles[2],
    uint32_t  *output);

int vtest_allocate_memory(int sock_fd,
                          uint32_t device_handle,
                          uint32_t memory_index,
                          VkDeviceSize size,
                          uint32_t *handle);

int vtest_bind_buffer_memory(int sock_fd,
                             uint32_t device_handle,
                             uint32_t buffer_handle,
                             uint32_t memory_handle,
                             uint32_t offset);

int vtest_write_descriptor_set(uint32_t sock_fd,
                               uint32_t device_handle,
                               uint32_t dst_set_handle,
                               uint32_t *buffer_handles,
                               const VkWriteDescriptorSet *info);

int vtest_create_fence(uint32_t sock_fd,
                       uint32_t device_handle,
                       uint32_t flags,
                       uint32_t *output);

int vtest_wait_for_fences(uint32_t sock_fd,
                          uint32_t device_handle,
                          uint32_t fence_count,
                          uint32_t wait_all,
                          uint64_t timeout,
                          uint32_t *handles);

int vtest_queue_submit(uint32_t sock_fd,
                       uint32_t device_handle,
                       uint32_t queue_handle,
                       uint32_t fence_handle,
                       const VkSubmitInfo *info,
                       uint32_t *wait_infos,
                       uint32_t *cmds_infos,
                       uint32_t *signal_handles);

int vtest_destroy_object(uint32_t sock_fd,
                         uint32_t device_handle,
                         uint32_t object_handle);

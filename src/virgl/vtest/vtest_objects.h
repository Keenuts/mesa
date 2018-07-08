#pragma once
#include <vulkan/vulkan.h>

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

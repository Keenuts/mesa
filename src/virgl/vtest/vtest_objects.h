           
#pragma once                
                            
#include <vulkan/vulkan.h>  
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
	const VkDescriptorSetAllocateInfo *create_info,
   uint32_t *handles,
	uint32_t  *output);
int vtest_create_shader_module(int sock_fd,
	uint32_t handle,
	const VkShaderModuleCreateInfo *create_info,
	uint32_t  *output);

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

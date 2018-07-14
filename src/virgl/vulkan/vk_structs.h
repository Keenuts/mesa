#ifndef VGL_STRUCTS
#define VGL_STRUCTS

#include <vulkan/vk_icd.h>

/* This struct describes a physical device */
struct virtiogpu
{
   uint32_t api_version;
   uint32_t driver_version;
};

struct vk_instance
{
   VK_LOADER_DATA loader_data;

   uint32_t physical_device_count;
   const VkAllocationCallbacks *allocators;
};

struct vk_physical_device
{
   VK_LOADER_DATA loader_data;

   uint32_t identifier;
   VkPhysicalDeviceSparseProperties sparse_properties;

   uint32_t queue_family_count;
   VkQueueFamilyProperties *queue_family_properties;

   /* All memory areas will e set as non-coherent */
   VkPhysicalDeviceMemoryProperties memory_properties;
};

struct vk_queue {
   VK_LOADER_DATA loader_data;

   uint32_t identifier;
   uint32_t queue_index;
   uint32_t family_index;
};

struct vk_descriptor_pool
{
   uint32_t identifier;
   const VkAllocationCallbacks *allocators;
};

struct vk_descriptor_set_layout {
   uint32_t identifier;
};

struct vk_descriptor_set {
   uint32_t identifier;
};

struct vk_shader_module {
   uint32_t identifier;
};

struct vk_pipeline_layout {
    uint32_t identifier;
    uint32_t max_set_count;
};

struct vk_pipeline {
    uint32_t identifier;
    struct vk_pipeline_layout *layout;
};

struct vk_device_memory {
   uint32_t identifier;
   uint32_t memory_index;
   VkDeviceSize size;
   VkMemoryPropertyFlags flags;

   uint64_t map_offset;
   uint64_t map_size;
   void *ptr;
};

struct vk_buffer {
   uint32_t identifier;
   uint64_t size;
   uint64_t usage;
   VkBufferCreateFlagBits flags;

   struct vk_device_memory *binding;
   uint64_t offset;
};

struct vk_compute_state {
   struct vk_pipeline *pipeline;
   struct vk_pipeline_layout *layout;

   uint32_t max_set_count;
   struct vk_descriptor_set **descriptor_sets;

   uint32_t dispatch_size[3];
};

struct vk_command_buffer {
   VK_LOADER_DATA loader_data;

   uint32_t identifier;
   struct vk_device *device;
   struct vk_command_pool *pool;

   VkCommandBufferLevel level;
   VkCommandBufferUsageFlagBits usage_flags;
   VkPipelineBindPoint bind_point;

   struct vk_compute_state compute_state;
};

struct vk_command_pool
{
   uint32_t identifier;
   const VkAllocationCallbacks *allocators;
};

struct vk_device
{
   VK_LOADER_DATA loader_data;
   struct vk_physical_device *physical_device;
   uint32_t identifier;
   uint32_t device_lost;

   uint32_t queue_count;
   struct vk_queue *queues;

   struct vk_command_pool command_pool;
};

#endif

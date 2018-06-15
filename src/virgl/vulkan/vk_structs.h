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
};

struct vk_queue {
   VK_LOADER_DATA loader_data;

   uint32_t identifier;
   uint32_t queue_index;
   uint32_t family_index;
};

struct vk_descriptor_pool
{
   const VkAllocationCallbacks *allocators;
};

struct vk_command_pool
{
   const VkAllocationCallbacks *allocators;
};

struct vk_device
{
   VK_LOADER_DATA loader_data;
   uint32_t identifier;
   uint32_t device_lost;

   uint32_t queue_count;
   struct vk_queue *queues;

   struct vk_descriptor_pool descriptor_pool;
   struct vk_command_pool command_pool;
};

#endif

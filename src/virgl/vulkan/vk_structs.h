#ifndef VGL_STRUCTS
#define VGL_STRUCTS

#include <vulkan/vk_icd.h>

#include "util/vector.h"

/* This struct describes a physical device */
struct virtiogpu
{
   uint32_t api_version;
   uint32_t driver_version;
};

struct vk_instance
{
   uint32_t physical_device_count;
   const VkAllocationCallbacks *allocators;
};

struct vk_physical_device
{
   uint32_t device_identifier;

};

struct vk_queue {
   VK_LOADER_DATA loader_data;
   /* might be useful to add flags in the future */
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
   uint32_t device_lost;

   struct vk_queue queue;
   struct vk_descriptor_pool descriptor_pool;
   struct vk_command_pool command_pool;
};

#endif

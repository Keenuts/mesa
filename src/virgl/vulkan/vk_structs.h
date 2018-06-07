#ifndef VGL_STRUCTS
#define VGL_STRUCTS

#include "util/vector.h"

struct virtiogpu
{
   uint32_t api_version;
   uint32_t driver_version;

   uint32_t available_devices;
   struct vector physical_devices;
};

struct virtiogpu physical_device;

struct vk_instance
{
   uint32_t physical_device_count;
   const VkAllocationCallbacks *allocators;
};

struct vk_physical_device
{
   uint32_t device_identifier;

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

   struct vk_descriptor_pool descriptor_pool;
   struct vk_command_pool command_pool;
};

#endif

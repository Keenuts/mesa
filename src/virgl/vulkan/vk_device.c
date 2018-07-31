#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "icd.h"
#include "memory.h"
#include "vgl_entrypoints.h"
#include "vk_structs.h"
#include "vtest/virgl_vtest.h"
#include "util/macros.h"

static int initialize_physical_device(struct vk_physical_device *device)
{
    int res;

    res = vtest_get_sparse_properties(icd_state.io_fd,
                                      device->identifier,
                                      &device->sparse_properties);
    if (res < 0) {
        return -1;
    }


    res = vtest_get_queue_family_properties(icd_state.io_fd,
                                            device->identifier,
                                            &device->queue_family_count,
                                            &device->queue_family_properties);
    if (res < 0) {
        return -2;
    }

    res = vtest_get_device_memory_properties(icd_state.io_fd,
                                             device->identifier,
                                             &device->memory_properties);
    if (res < 0) {
        return -3;
    }

    /* To avoid unecessary copies, we force the app to explicitly mark memory updates */
    for (uint32_t i = 0; i < device->memory_properties.memoryTypeCount; i++) {
        device->memory_properties.memoryTypes[i].propertyFlags &=
            ~VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    return 0;
}

int
initialize_physical_devices(void)
{
   int res;
   uint32_t device_count;


   list_init(&icd_state.physical_devices.list);

   res = vtest_get_physical_device_count(icd_state.io_fd, &device_count);
   if (res < 0) {
      return -1;
   }

   for (uint32_t i = 0; i < device_count; i++) {
      struct vk_physical_device_list *node = NULL;

      node = malloc(sizeof(*node));
      if (node == NULL) {
         return -2;
      }

      list_init(&node->list);
      node->device.identifier = i;

      if (initialize_physical_device(&node->device) != 0) {
          free(node);
          continue;
      }
      list_append(&icd_state.physical_devices.list, &node->list);
   }

   return 0;
}

VkResult
vgl_vkCreateInstance(UNUSED const VkInstanceCreateInfo *create_info,
                     const VkAllocationCallbacks *allocators,
                     VkInstance * instance)
{
   struct vk_instance *internal_instance = NULL;

   internal_instance = vk_malloc(sizeof(*internal_instance), allocators,
                                 VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);

   internal_instance->physical_device_count = UINT32_MAX;
   internal_instance->allocators = allocators;

   if (internal_instance == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   *instance = TO_HANDLE(internal_instance);
   return VK_SUCCESS;
}

void
vgl_vkDestroyInstance(VkInstance instance,
                      const VkAllocationCallbacks * allocators)
{
   vk_free(allocators, instance);

   return;
}

VkResult
vgl_vkEnumerateInstanceExtensionProperties(UNUSED const char *layer_name,
                                           uint32_t *property_count,
                                           UNUSED VkExtensionProperties *properties)
{
   *property_count = 0;

   return VK_SUCCESS;
}

VkResult
vgl_vkEnumerateInstanceVersion(uint32_t * pApiVersion)
{

   *pApiVersion = VK_MAKE_VERSION(1, 1, 0);
   return VK_SUCCESS;
}

VkResult
vgl_vkEnumeratePhysicalDevices(UNUSED VkInstance instance,
                               uint32_t * device_count,
                               VkPhysicalDevice * physical_devices)
{
   struct vk_physical_device_list *it = NULL;

   if (physical_devices == NULL) {
      *device_count = list_length(&icd_state.physical_devices.list);
      return VK_SUCCESS;
   }

   LIST_FOR_EACH(it, icd_state.physical_devices.list, list) {
      *physical_devices = TO_HANDLE(&it->device);
      physical_devices++;
   }

   return VK_SUCCESS;
}

void vgl_vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice handle,
                                             VkPhysicalDeviceMemoryProperties *props)
{
    struct vk_physical_device *device = NULL;

    device = FROM_HANDLE(device, handle);

    memcpy(props, &device->memory_properties, sizeof(*props));

}

void
vgl_vkGetPhysicalDeviceProperties(VkPhysicalDevice device,
                                  VkPhysicalDeviceProperties *properties)
{
   struct vk_physical_device *dev;


   dev = FROM_HANDLE(dev, device);

   properties->apiVersion = VK_MAKE_VERSION(1, 0, 0);
   properties->driverVersion = VK_MAKE_VERSION(1, 0, 0);
   properties->vendorID = REDHAT_VENDOR_ID;
   properties->deviceID = VIRTIOGPU_DEVICE_ID;
   properties->deviceType = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;

   strncpy(properties->deviceName, "VirtIO-gpu",
           VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);

   memcpy(&properties->sparseProperties,
          &dev->sparse_properties,
          sizeof(dev->sparse_properties));

   return;
}

void
vgl_vkGetPhysicalDeviceFeatures(UNUSED VkPhysicalDevice device,
                                VkPhysicalDeviceFeatures *features)
{
   memset(features, 0, sizeof(*features));
   return;
}

void
vgl_vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device,
                                             uint32_t *queue_count,
                                             VkQueueFamilyProperties *queues_properties)
{
   struct vk_physical_device *dev;


   dev = FROM_HANDLE(dev, device);

   if (queues_properties == NULL) {
      *queue_count = dev->queue_family_count;
      return;
   }

   memcpy(queues_properties,
          dev->queue_family_properties,
          sizeof(*queues_properties) * *queue_count);

   return;
}

VkResult
vgl_vkEnumerateDeviceExtensionProperties(UNUSED VkPhysicalDevice device,
                                         UNUSED const char *layer_name,
                                         UNUSED uint32_t * properties_count,
                                         UNUSED VkExtensionProperties * properties)
{
   /* we do not display any extensions for now */
   *properties_count = 0;

   return VK_SUCCESS;
}

static struct vk_physical_device*
get_physical_device_per_id(uint32_t device_id)
{
   struct vk_physical_device_list *it = NULL;

   LIST_FOR_EACH(it, icd_state.physical_devices.list, list) {
      if (0 == device_id) {
         return &it->device;
      }
      device_id--;
   }

   return NULL;
}

static int
initialize_vk_device(uint32_t physical_device_id,
                     const VkDeviceCreateInfo *info,
                     struct vk_device *dev)
{

   int res;
   uint32_t device_id, queue_count, id;

   res = vtest_create_device(icd_state.io_fd, physical_device_id, info, &device_id);
   if (res < 0) {
      return VK_ERROR_INITIALIZATION_FAILED;
   }

   dev->identifier = device_id;
   dev->physical_device = get_physical_device_per_id(physical_device_id);
   dev->device_lost = 0;

   if (NULL == dev->physical_device) {
      fprintf(stderr, "invalid physical device ID=%u\n", physical_device_id);
      return -1;
   }

   queue_count = 0;
   for (uint32_t i = 0; i < info->queueCreateInfoCount; i++) {
      queue_count += info->pQueueCreateInfos[i].queueCount;
   }

   dev->queues = calloc(queue_count, sizeof(*dev->queues));
   if (dev->queues == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   id = 0;
   for (uint32_t i = 0; i < info->queueCreateInfoCount; i++) {
      for (uint32_t j = 0; j < info->pQueueCreateInfos[i].queueCount; j++) {
         dev->queues[id].identifier = id;
         dev->queues[id].family_index = i;
         dev->queues[id].queue_index = j;
         dev->queues[id].device = dev;
         id++;
      }
   }
   dev->queue_count = queue_count;

   return VK_SUCCESS;
}

VkResult
vgl_vkCreateDevice(VkPhysicalDevice phys_device,
                   const VkDeviceCreateInfo *info,
                   const VkAllocationCallbacks *allocators,
                   VkDevice *device)
{

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_physical_device *p_device = NULL;

   if (info->enabledLayerCount != 0) {
      fprintf(stderr, "layer support is not implemented\n");
      return VK_ERROR_FEATURE_NOT_PRESENT;
   }

   if (info->enabledExtensionCount != 0) {
      fprintf(stderr, "extensions support is not implemented\n");
      return VK_ERROR_EXTENSION_NOT_PRESENT;
   }

   vk_device = vk_malloc(sizeof(*vk_device), allocators,
                         VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (vk_device == NULL) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   p_device = FROM_HANDLE(p_device, phys_device);
   res = initialize_vk_device(p_device->identifier, info, vk_device);
   if (res < 0) {
      return res;
   }

   *device = TO_HANDLE(vk_device);
   return VK_SUCCESS;
}

void
vgl_vkGetDeviceQueue(VkDevice device,
                     uint32_t queue_family_index,
                     uint32_t queue_index,
                     VkQueue *queue)
{
   struct vk_device *dev = NULL;
   uint32_t queue_count;

   *queue = VK_NULL_HANDLE;
   dev = FROM_HANDLE(dev, device);

   queue_count = dev->queue_count;
   for (uint32_t i = 0; i < queue_count; i++) {
      if (dev->queues[i].family_index != queue_family_index) {
         continue;
      }

      if (dev->queues[i].queue_index != queue_index) {
         continue;
      }

      *queue = TO_HANDLE(dev->queues + i);
      break;
   }

   return;
}

VkResult
vgl_vkMapMemory(UNUSED VkDevice device,
                VkDeviceMemory memory,
                VkDeviceSize offset,
                VkDeviceSize size,
                UNUSED VkMemoryMapFlags flags,
                void **ptr)
{
   struct vk_device_memory *vk_memory = NULL;

   vk_memory = FROM_HANDLE(vk_memory, memory);

   vk_memory->map_size = size;
   vk_memory->map_offset = offset;
   vk_memory->ptr = malloc(size);
   if (NULL == ptr) {
      return VK_ERROR_MEMORY_MAP_FAILED;
   }

   *ptr = vk_memory->ptr;
   return VK_SUCCESS;
}

VkResult
vgl_vkFlushMappedMemoryRanges(VkDevice device,
                              uint32_t range_count,
                              const VkMappedMemoryRange *ranges)
{

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_device_memory *vk_memory = NULL;

   vk_device = FROM_HANDLE(vk_device, device);

   for (uint32_t i = 0; i < range_count; i++) {
      vk_memory = FROM_HANDLE(vk_memory, ranges[i].memory);
      res = vtest_write_memory(icd_state.io_fd,
                               vk_device->identifier,
                               vk_memory->identifier,
                               ranges[i].offset,
                               ranges[i].size,
                               vk_memory->ptr);
      if (0 > res) {
         fprintf(stderr, "unable to flush a mapped memory range (%d)\n", -res);
      }

   }

   return VK_SUCCESS;
}

VkResult
vgl_vkInvalidateMappedMemoryRanges(VkDevice device,
                                   uint32_t range_count,
                                   const VkMappedMemoryRange *ranges)
{

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_device_memory *vk_memory = NULL;

   vk_device = FROM_HANDLE(vk_device, device);

   for (uint32_t i = 0; i < range_count; i++) {
      vk_memory = FROM_HANDLE(vk_memory, ranges[i].memory);
      res = vtest_read_memory(icd_state.io_fd,
                              vk_device->identifier,
                              vk_memory->identifier,
                              ranges[i].offset,
                              ranges[i].size,
                              vk_memory->ptr);
      if (0 > res) {
         fprintf(stderr, "unable to invalidate a mapped memory range (%d)\n", -res);
      }

   }

   return VK_SUCCESS;
}

void
vgl_vkUnmapMemory(UNUSED VkDevice device,
                  VkDeviceMemory memory)
{

   struct vk_device_memory *vk_memory = NULL;


   vk_memory = FROM_HANDLE(vk_memory, memory);
   free(vk_memory->ptr);
   vk_memory->ptr = NULL;

   return;
}

void
vgl_vkDestroyDevice(VkDevice device,
                    const VkAllocationCallbacks *allocators)
{
   int res;
   struct vk_device *vk_device = NULL;

   vk_device = FROM_HANDLE(vk_device, device);

   res = vtest_destroy_device(icd_state.io_fd, vk_device->identifier);
   if (0 != res) {
      fprintf(stderr, "device cleanup failed (%d).\n", res);
   }
   vk_free(allocators, vk_device);
}

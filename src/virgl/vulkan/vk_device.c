#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "common/macros.h"
#include "icd.h"
#include "memory.h"
#include "vgl_entrypoints.h"
#include "vk_structs.h"
#include "vtest/virgl_vtest.h"

static int initialize_physical_device(struct vk_physical_device *device)
{
    TRACE_IN();
    int res;

    res = vtest_get_sparse_properties(icd_state.io_fd,
                                      device->identifier,
                                      &device->sparse_properties);
    if (res < 0) {
        RETURN(-1);
    }


    res = vtest_get_queue_family_properties(icd_state.io_fd,
                                            device->identifier,
                                            &device->queue_family_count,
                                            &device->queue_family_properties);
    if (res < 0) {
        RETURN(-2);
    }

    res = vtest_get_device_memory_properties(icd_state.io_fd,
                                             device->identifier,
                                             &device->memory_properties);
    if (res < 0) {
        RETURN(-3);
    }

    /* To avoid unecessary copies, we force the app to explicitly mark memory updates */
    for (uint32_t i = 0; i < device->memory_properties.memoryTypeCount; i++) {
        device->memory_properties.memoryTypes[i].propertyFlags &=
            ~VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    RETURN(0);
}

int
initialize_physical_devices(void)
{
   int res;
   uint32_t device_count;

   TRACE_IN();

   list_init(&icd_state.physical_devices.list);

   res = vtest_get_physical_device_count(icd_state.io_fd, &device_count);
   if (res < 0) {
      RETURN(-1);
   }

   for (uint32_t i = 0; i < device_count; i++) {
      struct vk_physical_device_list *node = NULL;

      node = malloc(sizeof(*node));
      if (node == NULL) {
         RETURN(-2);
      }

      list_init(&node->list);
      node->device.identifier = i;

      if (initialize_physical_device(&node->device) != 0) {
          free(node);
          continue;
      }
      list_append(&icd_state.physical_devices.list, &node->list);
   }

   RETURN(0);
}

VkResult
vgl_vkCreateInstance(const VkInstanceCreateInfo * create_info,
                     const VkAllocationCallbacks * allocators,
                     VkInstance * instance)
{
   TRACE_IN();
   UNUSED_PARAMETER(create_info);

   struct vk_instance *internal_instance = NULL;

   internal_instance = vk_malloc(sizeof(*internal_instance), allocators,
                                 VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);

   internal_instance->physical_device_count = UINT32_MAX;
   internal_instance->allocators = allocators;

   if (internal_instance == NULL) {
      RETURN(VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   *instance = TO_HANDLE(internal_instance);
   RETURN(VK_SUCCESS);
}

void
vgl_vkDestroyInstance(VkInstance instance,
                      const VkAllocationCallbacks * allocators)
{
   TRACE_IN();

   UNUSED_PARAMETER(allocators);
   free(instance);

   RETURN();
}

VkResult
vgl_vkEnumerateInstanceExtensionProperties(const char *layer_name,
                                           uint32_t * property_count,
                                           VkExtensionProperties * properties)
{
   TRACE_IN();
   UNUSED_PARAMETER(layer_name);
   UNUSED_PARAMETER(properties);

   *property_count = 0;

   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkEnumerateInstanceVersion(uint32_t * pApiVersion)
{
   TRACE_IN();

   *pApiVersion = VK_MAKE_VERSION(1, 1, 0);
   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkEnumeratePhysicalDevices(VkInstance instance,
                               uint32_t * device_count,
                               VkPhysicalDevice * physical_devices)
{
   struct vk_physical_device_list *it = NULL;

   TRACE_IN();
   UNUSED_PARAMETER(instance);

   if (physical_devices == NULL) {
      *device_count = list_length(&icd_state.physical_devices.list);
      RETURN(VK_SUCCESS);
   }

   LIST_FOR_EACH(it, icd_state.physical_devices.list, list) {
      *physical_devices = TO_HANDLE(&it->device);
      physical_devices++;
   }

   RETURN(VK_SUCCESS);
}

void vgl_vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice handle,
                                             VkPhysicalDeviceMemoryProperties *props)
{
    struct vk_physical_device *device = NULL;

    TRACE_IN();
    device = FROM_HANDLE(device, handle);

    memcpy(props, &device->memory_properties, sizeof(*props));

    TRACE_OUT();
}

void
vgl_vkGetPhysicalDeviceProperties(VkPhysicalDevice device,
                                  VkPhysicalDeviceProperties *properties)
{
   struct vk_physical_device *dev;

   TRACE_IN();

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

   RETURN();
}

void
vgl_vkGetPhysicalDeviceFeatures(VkPhysicalDevice device,
                                VkPhysicalDeviceFeatures *features)
{
   TRACE_IN();

   UNUSED_PARAMETER(device);
   memset(features, 0, sizeof(*features));

   RETURN();
}

void
vgl_vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device,
                                             uint32_t *queue_count,
                                             VkQueueFamilyProperties *queues_properties)
{
   struct vk_physical_device *dev;

   TRACE_IN();

   dev = FROM_HANDLE(dev, device);

   if (queues_properties == NULL) {
      *queue_count = dev->queue_family_count;
      RETURN();
   }

   memcpy(queues_properties,
          dev->queue_family_properties,
          sizeof(*queues_properties) * *queue_count);

   RETURN();
}

VkResult
vgl_vkEnumerateDeviceExtensionProperties(VkPhysicalDevice device,
                                         const char *layer_name,
                                         uint32_t * properties_count,
                                         VkExtensionProperties * properties)
{
   TRACE_IN();
   UNUSED_PARAMETER(device);
   UNUSED_PARAMETER(layer_name);
   UNUSED_PARAMETER(properties_count);
   UNUSED_PARAMETER(properties);

   /* we do not display any extensions for now */
   *properties_count = 0;

   RETURN(VK_SUCCESS);
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
   TRACE_IN();

   int res;
   uint32_t device_id, queue_count, id;

   res = vtest_create_device(icd_state.io_fd, physical_device_id, info, &device_id);
   if (res < 0) {
      RETURN(VK_ERROR_INITIALIZATION_FAILED);
   }

   dev->identifier = device_id;
   dev->physical_device = get_physical_device_per_id(physical_device_id);
   dev->device_lost = 0;

   queue_count = 0;
   for (uint32_t i = 0; i < info->queueCreateInfoCount; i++) {
      queue_count += info->pQueueCreateInfos[i].queueCount;
   }

   dev->queues = calloc(queue_count, sizeof(*dev->queues));
   if (dev->queues == NULL) {
      RETURN(VK_ERROR_OUT_OF_HOST_MEMORY);
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

   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkCreateDevice(VkPhysicalDevice phys_device,
                   const VkDeviceCreateInfo *info,
                   const VkAllocationCallbacks *allocators,
                   VkDevice *device)
{
   TRACE_IN();

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_physical_device *p_device = NULL;

   //FIXME: use allocators
   UNUSED_PARAMETER(allocators);

   if (info->enabledLayerCount != 0) {
      fprintf(stderr, "layer support is not implemented\n");
      RETURN(VK_ERROR_FEATURE_NOT_PRESENT);
   }

   if (info->enabledExtensionCount != 0) {
      fprintf(stderr, "extensions support is not implemented\n");
      RETURN(VK_ERROR_EXTENSION_NOT_PRESENT);
   }

   vk_device = vk_malloc(sizeof(*vk_device), allocators,
                         VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE);
   if (vk_device == NULL) {
      RETURN(VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   p_device = FROM_HANDLE(p_device, phys_device);
   res = initialize_vk_device(p_device->identifier, info, vk_device);
   if (res < 0) {
      RETURN(res);
   }

   *device = TO_HANDLE(vk_device);
   RETURN(VK_SUCCESS);
}

void
vgl_vkGetDeviceQueue(VkDevice device,
                     uint32_t queue_family_index,
                     uint32_t queue_index,
                     VkQueue *queue)
{
   TRACE_IN();
   UNUSED_PARAMETER(device);

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

   RETURN();
}

VkResult
vgl_vkMapMemory(VkDevice device,
                VkDeviceMemory memory,
                VkDeviceSize offset,
                VkDeviceSize size,
                VkMemoryMapFlags flags,
                void **ptr)
{
   TRACE_IN();

   UNUSED_PARAMETER(device);
   UNUSED_PARAMETER(flags);

   struct vk_device_memory *vk_memory = NULL;

   vk_memory = FROM_HANDLE(vk_memory, memory);

   vk_memory->map_size = size;
   vk_memory->map_offset = offset;
   vk_memory->ptr = malloc(size);
   if (NULL == ptr) {
      RETURN(VK_ERROR_MEMORY_MAP_FAILED);
   }

   *ptr = vk_memory->ptr;
   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkFlushMappedMemoryRanges(VkDevice device,
                              uint32_t range_count,
                              const VkMappedMemoryRange *ranges)
{
   TRACE_IN();

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

   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkInvalidateMappedMemoryRanges(VkDevice device,
                                   uint32_t range_count,
                                   const VkMappedMemoryRange *ranges)
{
   TRACE_IN();

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

   RETURN(VK_SUCCESS);
}

void
vgl_vkUnmapMemory(VkDevice device,
                  VkDeviceMemory memory)
{
   TRACE_IN();

   UNUSED_PARAMETER(device);

   struct vk_device_memory *vk_memory = NULL;


   vk_memory = FROM_HANDLE(vk_memory, memory);
   free(vk_memory->ptr);
   vk_memory->ptr = NULL;

   RETURN();
}

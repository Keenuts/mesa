#include <stdio.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "common/macros.h"
#include "icd.h"
#include "memory.h"
#include "vgl_entrypoints.h"
#include "vk_structs.h"

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

/* We only expose one V-gpu for now */
static struct virtiogpu physical_device = {
   .api_version = VK_MAKE_VERSION(1, 1, 0),
   .driver_version = VK_MAKE_VERSION(0, 0, 1),
};

VkResult
vgl_vkEnumeratePhysicalDevices(VkInstance instance,
                               uint32_t * device_count,
                               VkPhysicalDevice * physical_devices)
{
   TRACE_IN();
   UNUSED_PARAMETER(instance);

   if (physical_devices == NULL) {
      *device_count = 1;
      RETURN(VK_SUCCESS);
   }

   *physical_devices = (void *) &physical_device;

   RETURN(VK_SUCCESS);
}

void
vgl_vkGetPhysicalDeviceProperties(VkPhysicalDevice device,
                                  VkPhysicalDeviceProperties * properties)
{
   TRACE_IN();

   struct virtiogpu *gpu = (struct virtiogpu *) device;
   VkPhysicalDeviceSparseProperties sparse_props;
   memset(&sparse_props, 0, sizeof(sparse_props));

   properties->apiVersion = gpu->api_version;
   properties->driverVersion = gpu->driver_version;
   properties->vendorID = REDHAT_VENDOR_ID;
   properties->deviceID = VIRTIOGPU_DEVICE_ID;
   properties->deviceType = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;

   strncpy(properties->deviceName, "VirtIO-gpu",
           VK_MAX_PHYSICAL_DEVICE_NAME_SIZE);
   properties->sparseProperties = sparse_props;

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


struct VkQueueFamilyProperties virgl_queue_families[] = {
   {
    .queueFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT |
    VK_QUEUE_TRANSFER_BIT,
    .queueCount = 1,
    .timestampValidBits = 0,    /* 0 means no support. 36 to 64 are valid values */
    .minImageTransferGranularity = {1, 1, 1},
    },
};

void
vgl_vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device,
                                             uint32_t * queue_count,
                                             VkQueueFamilyProperties *
                                             queues_properties)
{
   TRACE_IN();

   UNUSED_PARAMETER(device);

   *queue_count = 1;

   if (queues_properties == NULL) {
      RETURN();
   }

   memcpy(queues_properties, virgl_queue_families,
          sizeof(virgl_queue_families));
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

   /* we do not have any extensions, so none */
   *properties_count = 0;

   RETURN(VK_SUCCESS);
}

static struct vk_device *
initialize_vk_device(const VkAllocationCallbacks * allocators)
{
   struct vk_device *device = NULL;

   device = vk_calloc(sizeof(device), allocators, VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);

   return device;
}

VkResult
vgl_vkCreateDevice(VkPhysicalDevice phys_device,
                   const VkDeviceCreateInfo * info,
                   const VkAllocationCallbacks * allocators,
                   VkDevice * device)
{
   TRACE_IN();
   const VkDeviceQueueCreateInfo *queue_info = NULL;
   const VkPhysicalDeviceFeatures *feature_info = NULL;

   if (info->queueCreateInfoCount > 1) {
      RETURN(VK_ERROR_TOO_MANY_OBJECTS);
   }

   if (info->enabledLayerCount != 0) {
      RETURN(VK_ERROR_FEATURE_NOT_PRESENT);
   }

   if (info->enabledExtensionCount != 0) {
      RETURN(VK_ERROR_EXTENSION_NOT_PRESENT);
   }

   queue_info = info->pQueueCreateInfos;
   feature_info = info->pEnabledFeatures;

   if (queue_info->sType != VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO) {
      RETURN(VK_ERROR_INITIALIZATION_FAILED);
   }

   if (queue_info->queueFamilyIndex != 0 || queue_info->queueCount > 1) {
      RETURN(VK_ERROR_INITIALIZATION_FAILED);
   }

   UNUSED_PARAMETER(phys_device);
   UNUSED_PARAMETER(allocators);
   UNUSED_PARAMETER(device);
   UNUSED_PARAMETER(feature_info);

   *device = TO_HANDLE(initialize_vk_device(allocators));

   if (*device == NULL) {
      RETURN(VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   RETURN(VK_SUCCESS);
}

void
vgl_vkGetDeviceQueue(VkDevice device,
                     uint32_t queue_family_index,
                     uint32_t queue_index, VkQueue * queue)
{
   TRACE_IN();
   UNUSED_PARAMETER(device);

   *queue = VK_NULL_HANDLE;

   if (queue_family_index != 0) {
      RETURN();
   }

   if (queue_index != 0) {
      RETURN();
   }

   /* Just set some value if the app wants to check for NULL_HANDLE
    * but we do not use it for now */
   struct vk_device *dev = FROM_HANDLE(dev, device);
   *queue = TO_HANDLE(&dev->queue);

   RETURN();
}

VkResult
vgl_vkCreateDescriptorPool(VkDevice device,
                           const VkDescriptorPoolCreateInfo * create_info,
                           const VkAllocationCallbacks * allocators,
                           VkDescriptorPool * pool)
{
   TRACE_IN();

   UNUSED_PARAMETER(device);
   UNUSED_PARAMETER(create_info);
   UNUSED_PARAMETER(allocators);
   UNUSED_PARAMETER(pool);

   struct vk_device *dev = FROM_HANDLE(dev, device);

   /* for now, no pool. We are stupid and we will allocate/free every time */
   dev->descriptor_pool.allocators = allocators;

   *pool = TO_HANDLE(&dev->descriptor_pool);
   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkCreateCommandPool(VkDevice device,
                        const VkCommandPoolCreateInfo * create_info,
                        const VkAllocationCallbacks * allocators,
                        VkCommandPool * pool)
{
   TRACE_IN();

   UNUSED_PARAMETER(device);
   UNUSED_PARAMETER(create_info);
   UNUSED_PARAMETER(allocators);
   UNUSED_PARAMETER(pool);

   struct vk_device *dev = FROM_HANDLE(dev, device);

   /* for now, no pool. We are stupid and we will allocate/free every time */
   dev->command_pool.allocators = allocators;

   *pool = TO_HANDLE(&dev->command_pool);
   RETURN(VK_SUCCESS);
}

/*
VkResult vkCreateDescriptorSetLayout(VkDevice device,
                                     const VkDescriptorSetLayoutCreateInfo *info,
                                     const VkAllocationCallbacks *allocators,
                                     VkDescriptorSetLayout *layout)
{
   TRACE_IN();


   RETURN(VK_SUCCESS);
}
*/

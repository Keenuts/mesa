#include <vulkan/vulkan.h>

#include "common/macros.h"
#include "icd.h"
#include "memory.h"
#include "vgl_entrypoints.h"
#include "vk_structs.h"
#include "vtest/virgl_vtest.h"

/* file generated during compilation
   FIXME: should be done at the config
 */
#include "vtest/vtest_objects.h"

VkResult
vgl_vkCreateDescriptorSetLayout(VkDevice device,
                                const VkDescriptorSetLayoutCreateInfo *info,
                                const VkAllocationCallbacks *allocators,
                                VkDescriptorSetLayout *layout)
{
   TRACE_IN();

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_descriptor_layout *vk_layout = NULL;
   
   vk_device = FROM_HANDLE(vk_device, device);

   vk_layout = vk_malloc(sizeof(*vk_layout), allocators,
                         VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);

   if (vk_layout == NULL) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   res = vtest_create_descriptor_set_layout(icd_state.io_fd,
                                            vk_device->identifier,
                                            info,
                                            &vk_layout->identifier);
   if (res < 0) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   *layout = TO_HANDLE(vk_layout);

   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkCreateBuffer(VkDevice device,
                   const VkBufferCreateInfo *info,
                   const VkAllocationCallbacks *allocators,
                   VkBuffer *buffer)
{
   TRACE_IN();

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_buffer *vk_buffer = NULL;
   
   vk_device = FROM_HANDLE(vk_device, device);
   vk_buffer = vk_malloc(sizeof(*vk_buffer), allocators,
                         VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);

   if (vk_buffer == NULL) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   res = vtest_create_buffer(icd_state.io_fd,
                             vk_device->identifier,
                             info,
                             &vk_buffer->identifier);
   if (res < 0) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   *buffer = TO_HANDLE(vk_buffer);

   RETURN(VK_SUCCESS);
}

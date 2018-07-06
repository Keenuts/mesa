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
vgl_vkCreateDescriptorPool(VkDevice device,
                           const VkDescriptorPoolCreateInfo * create_info,
                           const VkAllocationCallbacks * allocators,
                           VkDescriptorPool *pool)
{
   TRACE_IN();

   struct vk_descriptor_pool *vk_pool = NULL;;
   struct vk_device *vk_device = NULL;

   vk_device = FROM_HANDLE(vk_device, device);

   vk_pool = vk_malloc(sizeof(*vk_pool), allocators,
                       VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   vtest_create_descriptor_pool(icd_state.io_fd,
                                vk_device->identifier,
                                create_info,
                                &vk_pool->identifier);

   *pool = TO_HANDLE(vk_pool);
   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkCreateDescriptorSetLayout(VkDevice device,
                                const VkDescriptorSetLayoutCreateInfo *info,
                                const VkAllocationCallbacks *allocators,
                                VkDescriptorSetLayout *layout)
{
   TRACE_IN();

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_descriptor_set_layout *vk_layout = NULL;

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
vgl_vkAllocateDescriptorSets(VkDevice device,
                         const VkDescriptorSetAllocateInfo *info,
                         VkDescriptorSet *vk_handles)
{
   TRACE_IN();

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_descriptor_pool *vk_pool = NULL;
   uint32_t *handles = NULL;

   if (0 == info->descriptorSetCount) {
      RETURN(VK_SUCCESS);
   }

   vk_device = FROM_HANDLE(vk_device, device);
   vk_pool = FROM_HANDLE(vk_pool, info->descriptorPool);

   /* Converting VK handles to VGL handles */
   handles = alloca(sizeof(*handles) * info->descriptorSetCount);
   for (uint32_t i = 0; i < info->descriptorSetCount; i++) {
      struct vk_descriptor_set_layout *layout = NULL;

      layout = FROM_HANDLE(layout, info->pSetLayouts[i]);
      handles[i] = layout->identifier;
   }

   res = vtest_allocate_descriptor_sets(icd_state.io_fd,
                                        vk_device->identifier,
                                        vk_pool->identifier,
                                        info,
                                        handles,
                                        handles);
   if (0 > res) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   for (uint32_t i = 0; i < info->descriptorSetCount; i++) {
      vk_handles[i] = TO_HANDLE((uintptr_t)handles[i]);
   }

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

   if (NULL == vk_buffer) {
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

VkResult
vgl_vkCreateShaderModule(VkDevice device,
                         const VkShaderModuleCreateInfo *info,
                         const VkAllocationCallbacks *allocators,
                         VkShaderModule *shader_module)
{
   TRACE_IN();
   int res;
   struct vk_device *vk_device = NULL;
   struct vk_shader_module *vk_shader_module = NULL;

   vk_device = FROM_HANDLE(vk_device, device);
   vk_shader_module = vk_malloc(sizeof(*vk_shader_module), allocators,
                                VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (NULL == vk_shader_module) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   res = vtest_create_shader_module(icd_state.io_fd,
                                    vk_device->identifier,
                                    info,
                                    &vk_shader_module->identifier);
   if (res < 0) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   *shader_module = TO_HANDLE(vk_shader_module);
   RETURN(VK_SUCCESS);
}

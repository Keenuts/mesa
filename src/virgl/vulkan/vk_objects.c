#include <string.h>
#include <vulkan/vulkan.h>

#include "common/macros.h"
#include "icd.h"
#include "memory.h"
#include "vgl_entrypoints.h"
#include "vk_structs.h"
#include "vtest/virgl_vtest.h"
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
   struct vk_descriptor_set *vk_sets = NULL;
   uint32_t *layout_handles = NULL;
   uint32_t *output_handles = NULL;

   if (0 == info->descriptorSetCount) {
      RETURN(VK_SUCCESS);
   }

   vk_device = FROM_HANDLE(vk_device, device);
   vk_pool = FROM_HANDLE(vk_pool, info->descriptorPool);

   //FIXME: use allocation pool
   vk_sets = malloc(sizeof(*vk_sets) * info->descriptorSetCount);
   if (NULL == vk_sets) {
      RETURN(VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   layout_handles = alloca(sizeof(uint32_t) * info->descriptorSetCount);
   output_handles = alloca(sizeof(uint32_t) * info->descriptorSetCount);

   /* Converting VK handles to VGL handles */
   for (uint32_t i = 0; i < info->descriptorSetCount; i++) {
      struct vk_descriptor_set_layout *layout = NULL;
      layout = FROM_HANDLE(layout, info->pSetLayouts[i]);
      layout_handles[i] = layout->identifier;
   }

   res = vtest_allocate_descriptor_sets(icd_state.io_fd,
                                        vk_device->identifier,
                                        vk_pool->identifier,
                                        info,
                                        layout_handles,
                                        output_handles);
   if (0 > res) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   for (uint32_t i = 0; i < info->descriptorSetCount; i++) {
      vk_sets[i].identifier = output_handles[i];
      vk_handles[i] = TO_HANDLE(vk_sets + i);
   }

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

VkResult
vgl_vkCreatePipelineLayout(VkDevice device,
                           const VkPipelineLayoutCreateInfo *create_info,
                           const VkAllocationCallbacks *allocators,
                           VkPipelineLayout *pipeline_layout)
{
   TRACE_IN();
   int res;
   struct vk_device *vk_device = NULL;
   struct vk_pipeline_layout *vk_pipeline_layout = NULL;
   struct vk_descriptor_set_layout *vk_layout = NULL;
   uint32_t *set_handles = NULL;

   vk_device = FROM_HANDLE(vk_device, device);
   vk_pipeline_layout = vk_malloc(sizeof(*vk_pipeline_layout), allocators,
                                  VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (NULL == vk_pipeline_layout) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   set_handles = alloca(sizeof(*set_handles) * create_info->setLayoutCount);
   for (uint32_t i = 0; i < create_info->setLayoutCount; i++) {
      vk_layout = FROM_HANDLE(vk_layout, create_info->pSetLayouts[i]);
      set_handles[i] = vk_layout->identifier;
   }

   res = vtest_create_pipeline_layout(icd_state.io_fd,
                                      vk_device->identifier,
                                      create_info,
                                      set_handles,
                                      &vk_pipeline_layout->identifier);
   if (res < 0) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   vk_pipeline_layout->max_set_count = create_info->setLayoutCount;
   *pipeline_layout = TO_HANDLE(vk_pipeline_layout);
   RETURN(VK_SUCCESS);
}

static VkResult create_compute_pipeline(const struct vk_device *vk_device,
                                        const VkComputePipelineCreateInfo *info,
                                        struct vk_pipeline *vk_pipeline)
{
   if (VK_PIPELINE_CREATE_DERIVATIVE_BIT & info->flags) {
      fprintf(stderr, "derivative pipelines not supported yet.\n");
      return VK_ERROR_FEATURE_NOT_PRESENT;
   }

   int res;
   struct vk_pipeline_layout   *layout;
   struct vk_shader_module     *shader_module;
   uint32_t handles[2];

   layout = FROM_HANDLE(layout, info->layout);
   shader_module = FROM_HANDLE(shader_module, info->stage.module);

   handles[0] = layout->identifier;
   handles[1] = shader_module->identifier;

   res = vtest_create_compute_pipelines(icd_state.io_fd,
                                        vk_device->identifier,
                                        info,
                                        handles,
                                        &vk_pipeline->identifier);
   if (res < 0) {
      return VK_ERROR_INITIALIZATION_FAILED;
   }

   vk_pipeline->layout = layout;
   return VK_SUCCESS;
}

VkResult
vgl_vkCreateComputePipelines(VkDevice device,
                             VkPipelineCache pipeline_cache,
                             uint32_t create_info_count,
                             const VkComputePipelineCreateInfo *create_info,
                             const VkAllocationCallbacks *allocators,
                             VkPipeline *pipeline)
{
    TRACE_IN();

    VkResult res;
    struct vk_device *vk_device = NULL;
    struct vk_pipeline *vk_pipelines = NULL;

    if (pipeline_cache != VK_NULL_HANDLE) {
        fprintf(stderr, "pipeline cache not supported for now.\n");
        RETURN(VK_ERROR_FEATURE_NOT_PRESENT);
    }

    vk_device = FROM_HANDLE(vk_device, device);
    vk_pipelines = vk_malloc(sizeof(*vk_pipelines) * create_info_count,
                             allocators,
                             VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);

    if (NULL == vk_pipelines) {
        RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
    }

    for (uint32_t i = 0; i < create_info_count; i++) {
        res = create_compute_pipeline(vk_device,
                                      create_info + i,
                                      vk_pipelines + 1);
        if (0 != res) {
            free(vk_pipelines);
            RETURN(res);
        }
    }

    for (uint32_t i = 0; i < create_info_count; i++) {
        pipeline[i] = TO_HANDLE(vk_pipelines + i);
    }
    RETURN(VK_SUCCESS);
}

VkResult
vgl_vkAllocateMemory(VkDevice device,
                     const VkMemoryAllocateInfo *info,
                     const VkAllocationCallbacks *allocators,
                     VkDeviceMemory *output)
{
   TRACE_IN();

   int res;
   struct vk_device *vk_device = NULL;
   struct vk_device_memory *vk_memory = NULL;
   struct vk_physical_device *phys_device = NULL;
   VkMemoryType mem_type;

   vk_device = FROM_HANDLE(vk_device, device);
   phys_device = vk_device->physical_device;

   if (info->memoryTypeIndex >= phys_device->memory_properties.memoryTypeCount) {
      RETURN(VK_ERROR_INVALID_EXTERNAL_HANDLE);
   }

   vk_memory = vk_calloc(sizeof(*vk_memory),
                         allocators,
                         VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (NULL == vk_memory) {
      RETURN(VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   res = vtest_allocate_memory(icd_state.io_fd,
                               vk_device->identifier,
                               info->memoryTypeIndex,
                               info->allocationSize,
                               &vk_memory->identifier);
   if (0 > res) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   mem_type = phys_device->memory_properties.memoryTypes[info->memoryTypeIndex];
   vk_memory->memory_index = info->memoryTypeIndex;
   vk_memory->size = info->allocationSize;
   vk_memory->flags = mem_type.propertyFlags;

   *output = TO_HANDLE(vk_memory);
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
      RETURN(VK_ERROR_OUT_OF_HOST_MEMORY);
   }

   memset(vk_buffer, 0, sizeof(*vk_buffer));
   res = vtest_create_buffer(icd_state.io_fd,
                             vk_device->identifier,
                             info,
                             &vk_buffer->identifier);
   if (0 > res) {
      RETURN(VK_ERROR_DEVICE_LOST);
   }

   vk_buffer->size = info->size;
   vk_buffer->usage = info->usage;
   vk_buffer->flags = info->flags;

   *buffer = TO_HANDLE(vk_buffer);

   RETURN(VK_SUCCESS);
}

VkResult
vgl_vkBindBufferMemory(VkDevice device,
                       VkBuffer buffer,
                       VkDeviceMemory memory,
                       VkDeviceSize offset)
{
   TRACE_IN();
   int res;
   struct vk_device *vk_device = NULL;
   struct vk_buffer *vk_buffer = NULL;
   struct vk_device_memory *vk_memory = NULL;

   vk_device = FROM_HANDLE(vk_device, device);
   vk_buffer = FROM_HANDLE(vk_buffer, buffer);
   vk_memory = FROM_HANDLE(vk_memory, memory);

   if (vk_buffer->flags & (VK_BUFFER_CREATE_SPARSE_BINDING_BIT |
                           VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT |
                           VK_BUFFER_CREATE_SPARSE_ALIASED_BIT)) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   if (offset > vk_memory->size
     || vk_buffer->size > vk_memory->size - offset
     || NULL != vk_buffer->binding) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   if (((vk_buffer->flags & VK_BUFFER_CREATE_PROTECTED_BIT) == 0)
       != ((vk_memory->flags & VK_MEMORY_PROPERTY_PROTECTED_BIT) == 0)) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   res = vtest_bind_buffer_memory(icd_state.io_fd,
                                  vk_device->identifier,
                                  vk_buffer->identifier,
                                  vk_memory->identifier,
                                  offset);
   if (0 > res) {
      RETURN(VK_ERROR_OUT_OF_DEVICE_MEMORY);
   }

   vk_buffer->binding = vk_memory;
   vk_buffer->offset = offset;
   RETURN(VK_SUCCESS);
}

void
vgl_vkUpdateDescriptorSets(VkDevice device,
                           uint32_t write_count,
                           const VkWriteDescriptorSet *write_info,
                           uint32_t copy_count,
                           const VkCopyDescriptorSet *copy_info)
{
   TRACE_IN();

   int res;
   struct vk_device *vk_device = NULL;

   struct vk_descriptor_set *descriptor = NULL;
   struct vk_buffer *vk_buffer = NULL;
   uint32_t *buffer_handles = NULL;

   /* Usage check
    * For now only one specific case is supported
    */
   if (0 != copy_count) {
      fprintf(stderr, "descriptor copy not implemented for now\n");
      RETURN();
   }

   for (uint32_t i = 0; i < write_count; i++) {
      if (write_info[i].descriptorType & ~VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) {
         fprintf(stderr, "Only VK_DESCRIPTOR_TYPE_STORAGE_BUFFER are supported\n");
         RETURN();
      }
   }

   /* splitting into simple sub-commands */
   vk_device = FROM_HANDLE(vk_device, device);
   for (uint32_t i = 0; i < write_count; i++) {
      descriptor = FROM_HANDLE(descriptor, write_info[i].dstSet);

      buffer_handles = alloca(sizeof(uint32_t) * write_info[i].descriptorCount);
      for (uint32_t j = 0; j < write_info[i].descriptorCount; j++) {
         vk_buffer = FROM_HANDLE(vk_buffer, write_info[i].pBufferInfo[j].buffer);
         buffer_handles[j] = vk_buffer->identifier;
      }

      res = vtest_write_descriptor_set(icd_state.io_fd,
                                       vk_device->identifier,
                                       descriptor->identifier,
                                       buffer_handles,
                                       write_info + i);
      if (0 > res) {
         fprintf(stderr, "vtest_update_descriptor_sets failed (%d)\n", res);
         // One might fail, but we will still send the others
         continue;
      }
   }

   UNUSED_PARAMETER(copy_info);
   RETURN();
}

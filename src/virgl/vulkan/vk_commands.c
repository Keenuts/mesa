#include <string.h>
#include <vulkan/vulkan.h>

#include "icd.h"
#include "memory.h"
#include "util/macros.h"
#include "vgl_entrypoints.h"
#include "vk_structs.h"
#include "vtest/virgl_vtest.h"
#include "vtest/vtest_commands.h"
#include "vtest/vtest_objects.h"


VkResult
vgl_vkCreateCommandPool(VkDevice device,
                        const VkCommandPoolCreateInfo *info,
                        const VkAllocationCallbacks *allocators,
                        VkCommandPool *pool)
{

   int res;
   struct vk_command_pool *vk_pool = NULL;;
   struct vk_device *vk_device = NULL;
   struct vtest_command_pool_create_info vtest_info;

   vk_device = FROM_HANDLE(vk_device, device);

   vk_pool = vk_calloc(sizeof(*vk_pool), allocators,
                       VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);

   vtest_info.device_handle = vk_device->identifier;
   vtest_info.flags = info->flags;
   vtest_info.queue_family_index = info->queueFamilyIndex;

   res = vtest_create_command_pool(icd_state.io_fd,
                                   &vtest_info,
                                   &vk_pool->identifier);
   if (0 > res) {
      free(vk_pool);
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;
   }

   vk_pool->allocators = allocators;
   *pool = TO_HANDLE(vk_pool);
   return VK_SUCCESS;
}

VkResult
vgl_vkAllocateCommandBuffers(VkDevice device,
                             const VkCommandBufferAllocateInfo *info,
                             VkCommandBuffer *output)
{
   int res;
   struct vk_device *vk_device = NULL;
   struct vk_command_buffer *vk_cmds = NULL;
   struct vk_command_pool *vk_pool = NULL;
   struct vtest_command_buffer_allocate_info vtest_info;
   uint32_t *handles = NULL;

   vk_device = FROM_HANDLE(vk_device, device);
   vk_pool = FROM_HANDLE(vk_pool, info->commandPool);

   if (VK_COMMAND_BUFFER_LEVEL_SECONDARY == info->level) {
      fprintf(stderr, "only primary command buffers are supported for now\n");
      return VK_ERROR_FEATURE_NOT_PRESENT;
   }

   vk_cmds = vk_calloc(sizeof(*vk_cmds) * info->commandBufferCount,
                       vk_pool->allocators,
                       VK_SYSTEM_ALLOCATION_SCOPE_DEVICE);
   if (NULL == vk_cmds) {
      return VK_ERROR_OUT_OF_HOST_MEMORY;
   }

   handles = alloca(sizeof(uint32_t) * info->commandBufferCount);

   vtest_info.device_handle = vk_device->identifier;
   vtest_info.pool_handle = vk_pool->identifier;
   vtest_info.level = info->level;
   vtest_info.count = info->commandBufferCount;

   res = vtest_allocate_command_buffers(icd_state.io_fd,
                                        &vtest_info,
                                        handles);
   if (res > 0) {
      free(vk_cmds);
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;
   }

   for (uint32_t i = 0; i < info->commandBufferCount; i++) {
      vk_cmds[i].identifier = handles[i];
      vk_cmds[i].pool = vk_pool;
      vk_cmds[i].device = vk_device;
      vk_cmds[i].level = info->level;

      output[i] = TO_HANDLE(vk_cmds+ i);
   }

   return VK_SUCCESS;
}

VkResult
vgl_vkBeginCommandBuffer(VkCommandBuffer cmd,
                         const VkCommandBufferBeginInfo *info)
{
   struct vk_command_buffer *vk_cmd = NULL;

   vk_cmd = FROM_HANDLE(vk_cmd, cmd);

   if (VK_COMMAND_BUFFER_LEVEL_SECONDARY == vk_cmd->level) {
      fprintf(stderr, "only primary command buffers are supported for now\n");
      return VK_ERROR_FEATURE_NOT_PRESENT;
   }

   vk_cmd->usage_flags = info->flags;

   if (vk_cmd->compute_state.pipeline != NULL) {
      /* the command buffer as already be bound, thus
       * descriptors arrays have been allocated */
      free(vk_cmd->compute_state.descriptor_sets);
   }

   memset(&vk_cmd->compute_state, 0, sizeof(vk_cmd->compute_state));

   return VK_SUCCESS;
}

void
vgl_vkCmdBindPipeline(VkCommandBuffer buffer,
                  VkPipelineBindPoint bind_point,
                  VkPipeline pipeline)
{

   struct vk_command_buffer *vk_cmd = NULL;
   struct vk_pipeline *vk_pipeline = NULL;
   struct vk_compute_state *state = NULL;

   if (VK_PIPELINE_BIND_POINT_COMPUTE != bind_point) {
      fprintf(stderr, "non-compute pipeline not suported yet.\n");
      abort();
   }

   vk_pipeline = FROM_HANDLE(vk_pipeline, pipeline);
   vk_cmd = FROM_HANDLE(vk_cmd, buffer);


   state = &vk_cmd->compute_state;
   state->pipeline = vk_pipeline;
   state->layout = vk_pipeline->layout;
   state->max_set_count = vk_pipeline->layout->max_set_count;
   state->descriptor_sets = calloc(state->max_set_count,
                                   sizeof(*state->descriptor_sets));
   if (NULL == state->descriptor_sets) {
      /* other drivers do not allocate dynamicly, they have a MAX_SET.
       * This max is different for each implem. Thus, we cannot
       * rely on it too.
       * If malloc fails, the application will probaly crash, so for
       * now, we force it.
       */
      abort();
   }

}

void
vgl_vkCmdBindDescriptorSets(VkCommandBuffer buffer,
                            VkPipelineBindPoint bind_point,
                            VkPipelineLayout layout,
                            uint32_t first_set,
                            uint32_t descriptor_set_count,
                            const VkDescriptorSet *descriptor_sets,
                            UNUSED uint32_t dynamic_offset_count,
                            const uint32_t *dynamic_offsets)
{

   struct vk_command_buffer *vk_cmd = NULL;
   struct vk_pipeline_layout *vk_layout = NULL;
   struct vk_descriptor_set *vk_descriptor = NULL;
   struct vk_compute_state *state = NULL;
   uint32_t descriptor_count;

   if (VK_PIPELINE_BIND_POINT_COMPUTE != bind_point) {
      fprintf(stderr, "non-compute pipeline not suported yet.\n");
      abort();
   }

   if (NULL != dynamic_offsets) {
      fprintf(stderr, "dynamic offsets not supported yet.\n");
      abort();
   }

   vk_cmd = FROM_HANDLE(vk_cmd, buffer);
   vk_layout = FROM_HANDLE(vk_layout, layout);
   state = &vk_cmd->compute_state;

   vk_cmd->bind_point = bind_point;
   descriptor_count = first_set + descriptor_set_count;

   for (uint32_t i = 0; i < descriptor_count; i++) {
      vk_descriptor = FROM_HANDLE(vk_descriptor, descriptor_sets[i]);
      state->descriptor_sets[i] = vk_descriptor;
   }
}

void
vgl_vkCmdDispatch(VkCommandBuffer buffer,
                  uint32_t group_count_x,
                  uint32_t group_count_y,
                  uint32_t group_count_z)
{

   struct vk_command_buffer *vk_cmd = NULL;

   vk_cmd = FROM_HANDLE(vk_cmd, buffer);

   vk_cmd->compute_state.dispatch_size[0] = group_count_x;
   vk_cmd->compute_state.dispatch_size[1] = group_count_y;
   vk_cmd->compute_state.dispatch_size[2] = group_count_z;

}

VkResult
vgl_vkEndCommandBuffer(VkCommandBuffer cmd)
{

   int res;
   struct vk_command_buffer *vk_cmd = NULL;
   struct vk_compute_state *state = NULL;
   struct vtest_command_record_info record_info = { 0 };
   uint32_t *descriptor_handles;

   vk_cmd = FROM_HANDLE(vk_cmd, cmd);
   state = &vk_cmd->compute_state;

   record_info.device_handle = vk_cmd->device->identifier;
   record_info.pool_handle = vk_cmd->pool->identifier;
   record_info.cmd_handle = vk_cmd->identifier;
   record_info.pipeline_handle = state->pipeline->identifier;
   record_info.pipeline_layout_handle = state->layout->identifier;
   record_info.bind_point = vk_cmd->bind_point;
   memcpy(&record_info.dispatch_size,
          &state->dispatch_size,
          sizeof(uint32_t) * 3);

   record_info.descriptor_count = state->max_set_count;
   descriptor_handles = alloca(sizeof(uint32_t) * state->max_set_count);
   for (uint32_t i = 0; i < record_info.descriptor_count; i++) {
      descriptor_handles[i] = state->descriptor_sets[i]->identifier;
   }

   res = vtest_record_command(icd_state.io_fd, &record_info, descriptor_handles);
   if (0 > res) {
      return VK_ERROR_OUT_OF_DEVICE_MEMORY;
   }

   return VK_SUCCESS;
}

void
vgl_vkDestroyCommandPool(VkDevice device,
                         VkCommandPool pool,
                         const VkAllocationCallbacks *allocators)
{
   int res;
   struct vk_device *vk_device = NULL;
   struct vk_command_pool *vk_pool = NULL;

   vk_pool = FROM_HANDLE(vk_pool, pool);
   vk_device = FROM_HANDLE(vk_device, device);
   res = vtest_destroy_object(icd_state.io_fd,
                              vk_device->identifier,
                              vk_pool->identifier);

   if (0 != res) {
      fprintf(stderr,
               "%s: error while deleting the object id=%d\n",
              __func__,
              vk_pool->identifier);
   }
   vk_free(allocators, vk_pool);
}

#include <string.h>
#include <vulkan/vulkan.h>

#include "common/macros.h"
#include "virgl_vtest.h"
#include "vtest_protocol.h"
#include "vtest_objects.h"

int vtest_create_descriptor_set_layout(int sock_fd,
   uint32_t handle,
   const VkDescriptorSetLayoutCreateInfo *create_info,
   uint32_t  *output)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_descriptor_set_layout_intro intro;
   struct payload_create_descriptor_set_layout_pBindings pBindings;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DESCRIPTOR_SET_LAYOUT, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.bindingCount = create_info->bindingCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   for (uint32_t i = 0; i < create_info->bindingCount; i++) {
      pBindings.binding = create_info->pBindings[i].binding;
      pBindings.descriptorType = create_info->pBindings[i].descriptorType;
      pBindings.descriptorCount = create_info->pBindings[i].descriptorCount;
      pBindings.stageFlags = create_info->pBindings[i].stageFlags;
      res = virgl_block_write(sock_fd, &pBindings, sizeof(pBindings));
      CHECK_IO_RESULT(res, sizeof(pBindings));
   }

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}


int vtest_allocate_descriptor_sets(int sock_fd,
   uint32_t handle,
   uint32_t pool_handle,
   const VkDescriptorSetAllocateInfo *create_info,
   uint32_t *handles,
   uint32_t  *output)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_allocate_descriptor_sets_intro intro;

   INITIALIZE_HDR(cmd, VCMD_VK_ALLOCATE_DESCRIPTORS, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = handle;
   intro.descriptorPool = pool_handle;
   intro.descriptorSetCount = create_info->descriptorSetCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   res = virgl_block_write(sock_fd, handles, sizeof(*handles) * intro.descriptorSetCount);
   CHECK_IO_RESULT(res, sizeof(*handles) * intro.descriptorSetCount);

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   if (result.error_code != 0) {
      RETURN(result.error_code);
   }

   res = virgl_block_read(sock_fd, output, sizeof(*output) * result.result);
   CHECK_IO_RESULT(res, result.result * sizeof(uint32_t));

   RETURN(result.error_code);
}

int vtest_create_shader_module(int sock_fd,
   uint32_t handle,
   const VkShaderModuleCreateInfo *create_info,
   uint32_t  *output)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_shader_module_intro intro;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_SHADER_MODULE, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.codeSize = create_info->codeSize;

   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   res = virgl_block_write(sock_fd, create_info->pCode, create_info->codeSize);
   CHECK_IO_RESULT(res, create_info->codeSize);

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}

int vtest_create_descriptor_pool(int sock_fd,
   uint32_t handle,
   const VkDescriptorPoolCreateInfo *create_info,
   uint32_t  *output)
{

   int res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_descriptor_pool_intro intro;
   struct payload_create_descriptor_pool_pPoolSizes pPoolSizes;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DESCRIPTOR_POOL, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.maxSets = create_info->maxSets;
   intro.poolSizeCount = create_info->poolSizeCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   for (uint32_t i = 0; i < create_info->poolSizeCount; i++) {
      pPoolSizes.type = create_info->pPoolSizes[i].type;
      pPoolSizes.descriptorCount = create_info->pPoolSizes[i].descriptorCount;
      res = virgl_block_write(sock_fd, &pPoolSizes, sizeof(pPoolSizes));
      CHECK_IO_RESULT(res, sizeof(pPoolSizes));
   }

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}

int vtest_create_pipeline_layout(int sock_fd,
    uint32_t handle,
    const VkPipelineLayoutCreateInfo *create_info,
    uint32_t *set_handles,
    uint32_t  *output)
{
   TRACE_IN();

   int res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_pipeline_layout_intro intro;
   struct payload_create_pipeline_layout_pPushConstantRanges pPushConstantRanges;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_PIPELINE_LAYOUT, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.setLayoutCount = create_info->setLayoutCount;
   intro.pushConstantRangeCount = create_info->pushConstantRangeCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   /* writing first array */
   res = virgl_block_write(sock_fd, set_handles,
         sizeof(*set_handles) * create_info->setLayoutCount);
   CHECK_IO_RESULT(res, sizeof(*set_handles) * create_info->setLayoutCount);

   /* writting second array */
   for (uint32_t i = 0; i < create_info->pushConstantRangeCount; i++) {
      pPushConstantRanges.stageFlags = create_info->pPushConstantRanges[i].stageFlags;
      pPushConstantRanges.offset = create_info->pPushConstantRanges[i].offset;
      pPushConstantRanges.size = create_info->pPushConstantRanges[i].size;

      res = virgl_block_write(sock_fd, &pPushConstantRanges, sizeof(pPushConstantRanges));
      CHECK_IO_RESULT(res, sizeof(pPushConstantRanges));
   }

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}

int vtest_create_compute_pipelines(int sock_fd,
    uint32_t device_handle,
    const VkComputePipelineCreateInfo *info,
    uint32_t handles[2],
    uint32_t  *output)
{
   TRACE_IN();

   int res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_compute_pipelines_intro intro;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_COMPUTE_PIPELINES, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = device_handle;
   intro.flags = info->flags;
   intro.layout = handles[0];

   intro.stage_flags = info->stage.flags;
   intro.stage_stage = info->stage.stage;
   intro.stage_module = handles[1];
   //FIXME: entrypoint can be UTF-8.
   intro.entrypoint_len = strlen(info->stage.pName) + 1;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   res = virgl_block_write(sock_fd, info->stage.pName, intro.entrypoint_len);
   CHECK_IO_RESULT(res, intro.entrypoint_len);

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}

int vtest_allocate_memory(int sock_fd,
                          uint32_t device_handle,
                          uint32_t memory_index,
                          VkDeviceSize size,
                          uint32_t *handle)
{
   int res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_allocate_memory intro;

   TRACE_IN();
   INITIALIZE_HDR(cmd, VCMD_VK_ALLOCATE_MEMORY, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   intro.handle = device_handle;
   intro.memory_index = memory_index;
   intro.device_size = size;

   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   *handle = result.result;

   RETURN(result.error_code);
}

int vtest_create_buffer(int sock_fd,
                        uint32_t handle,
                        const VkBufferCreateInfo *create_info,
                        uint32_t  *output)
{
   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_buffer_intro intro;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_BUFFER, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   memset(&intro, 0, sizeof(intro));
   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.size = create_info->size;
   intro.usage = create_info->usage;
   intro.sharingMode = create_info->sharingMode;
   intro.queueFamilyIndexCount = create_info->queueFamilyIndexCount;

   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

   if (0 != intro.queueFamilyIndexCount) {
      res = virgl_block_write(sock_fd,
                              create_info->pQueueFamilyIndices,
                              sizeof(uint32_t) * intro.queueFamilyIndexCount);
      CHECK_IO_RESULT(res, sizeof(uint32_t) * intro.queueFamilyIndexCount);
   }

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   *output = result.result;
   RETURN(result.error_code);
}

int vtest_bind_buffer_memory(int sock_fd,
                             uint32_t device_handle,
                             uint32_t buffer_handle,
                             uint32_t memory_handle,
                             uint32_t offset)
{
   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_bind_buffer_memory payload;

   INITIALIZE_HDR(cmd, VCMD_VK_BIND_BUFFER_MEMORY, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   memset(&payload, 0, sizeof(payload));
   payload.device_handle = device_handle;
   payload.buffer_handle = buffer_handle;
   payload.memory_handle = memory_handle;
   payload.offset = offset;

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   RETURN(result.error_code);
}

int vtest_write_descriptor_set(uint32_t sock_fd,
                               uint32_t device_handle,
                               uint32_t dst_set_handle,
                               uint32_t *buffer_handles,
                               const VkWriteDescriptorSet *info)
{
   int res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_write_descriptor_set payload = { 0 };
   struct payload_write_descriptor_set_buffer buffer_info = { 0 };

   TRACE_IN();

   INITIALIZE_HDR(cmd, VCMD_VK_WRITE_DESCRIPTOR_SET, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_handle = device_handle;
   payload.dst_set = dst_set_handle;
   payload.dst_binding = info->dstBinding;
   payload.dst_array_element = info->dstArrayElement;
   payload.descriptor_type = info->descriptorType;
   payload.descriptor_count = info->descriptorCount;

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   for (uint32_t i = 0; i < info->descriptorCount; i++) {
      buffer_info.buffer_handle = buffer_handles[i];
      buffer_info.offset = info->pBufferInfo[i].offset;
      buffer_info.range = info->pBufferInfo[i].range;

      res = virgl_block_write(sock_fd, &buffer_info, sizeof(buffer_info));
      CHECK_IO_RESULT(res, sizeof(buffer_info));
   }

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   RETURN(result.error_code);
}

int vtest_create_fence(uint32_t sock_fd,
                       uint32_t device_handle,
                       uint32_t flags,
                       uint32_t *output)
{
   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_create_fence payload = { 0 };

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_FENCE, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_handle = device_handle;
   payload.flags = flags;

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   *output = result.result;

   RETURN(result.error_code);
}

int vtest_wait_for_fences(uint32_t sock_fd,
                          uint32_t device_handle,
                          uint32_t fence_count,
                          uint32_t wait_all,
                          uint64_t timeout,
                          uint32_t *handles)
{
   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_wait_for_fences payload = { 0 };

   INITIALIZE_HDR(cmd, VCMD_VK_WAIT_FOR_FENCES, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_handle = device_handle;
   payload.fence_count = fence_count;
   payload.wait_all = wait_all;
   payload.timeout = timeout;

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_write(sock_fd, handles, sizeof(uint32_t) * fence_count);
   CHECK_IO_RESULT(res, sizeof(uint32_t) * fence_count);

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   RETURN(result.error_code);
}

int vtest_queue_submit(uint32_t sock_fd,
                       uint32_t device_handle,
                       uint32_t queue_handle,
                       uint32_t fence_handle,
                       const VkSubmitInfo *info,
                       uint32_t *wait_infos,
                       uint32_t *cmds_infos,
                       uint32_t *signal_handles)
{
   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_queue_submit payload = { 0 };

   INITIALIZE_HDR(cmd, VCMD_VK_QUEUE_SUBMIT, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_handle = device_handle;
   payload.queue_handle = queue_handle;
   payload.fence_handle = fence_handle;
   payload.wait_count = info->waitSemaphoreCount;
   payload.cmd_count = info->commandBufferCount;
   payload.signal_count = info->signalSemaphoreCount;

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_write(sock_fd,
                           wait_infos,
                           sizeof(uint32_t) * info->waitSemaphoreCount * 2);
   CHECK_IO_RESULT(res, sizeof(uint32_t) * info->waitSemaphoreCount * 2);
   res = virgl_block_write(sock_fd,
                           cmds_infos,
                           sizeof(uint32_t) * info->commandBufferCount * 2);
   CHECK_IO_RESULT(res, sizeof(uint32_t) * info->commandBufferCount * 2);
   res = virgl_block_write(sock_fd,
                           signal_handles,
                           sizeof(uint32_t) * info->signalSemaphoreCount);
   CHECK_IO_RESULT(res, sizeof(uint32_t) * info->signalSemaphoreCount);

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   RETURN(result.error_code);
}

int vtest_destroy_object(uint32_t sock_fd,
                         uint32_t device_handle,
                         uint32_t object_handle)
{
   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;
   struct payload_destroy_object payload;

   INITIALIZE_HDR(cmd, VCMD_VK_DESTROY_OBJECT, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_handle = device_handle;
   payload.object_handle = object_handle;

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   return 0;
}

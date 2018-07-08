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

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DESCRIPTOR_LAYOUT, sizeof(cmd));
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

   intro.handle = handle;
   intro.flags = create_info->flags;
   intro.size = create_info->size;
   intro.usage = create_info->usage;
   intro.sharingMode = create_info->sharingMode;
   intro.queueFamilyIndexCount = create_info->queueFamilyIndexCount;
   res = virgl_block_write(sock_fd, &intro, sizeof(intro));
   CHECK_IO_RESULT(res, sizeof(intro));

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
    intro.stage_stage = info->stage.flags;
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

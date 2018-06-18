#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#include "common/macros.h"
#include "virgl_vtest.h"
#include "vtest_protocol.h"

int vtest_create_descriptor_layout(int sock_fd,
                                   uint32_t device_id,
                                   const VkDescriptorSetLayoutCreateInfo *info,
                                   uint32_t *descriptor_set_id)
{
   int res;
   struct vtest_hdr cmd;
   struct vtest_payload_descriptor_set_layout layout;
   struct vtest_payload_descriptor_set_layout_bindings binding;
   struct vtest_result result;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DESCRIPTOR_LAYOUT, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   layout.device_id = device_id;
   layout.flags = info->flags;
   layout.binding_count = info->bindingCount;

   res = virgl_block_write(sock_fd, &layout, sizeof(layout));
   CHECK_IO_RESULT(res, sizeof(layout));

   for (uint32_t i = 0; i < layout.binding_count; i++) {
      binding.binding          = info->pBindings[i].binding;
      binding.descriptor_type  = info->pBindings[i].descriptorType;
      binding.descriptor_count = info->pBindings[i].descriptorCount;
      binding.stage_flags      = info->pBindings[i].stageFlags;

      res = virgl_block_write(sock_fd, &binding, sizeof(binding));
      CHECK_IO_RESULT(res, sizeof(binding));

      if (info->pBindings[i].pImmutableSamplers == NULL)
         continue;

      fprintf(stderr, "immutable samplers initialization non supported yet\n");
      RETURN(-1);
   }

   /* reading the object ID allocated by virgl (or the error code) */
   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   *descriptor_set_id = result.result;
   RETURN(result.error_code);
}

int vtest_create_buffer(int sock_fd,
                        uint32_t device_id,
                        const VkBufferCreateInfo *info,
                        uint32_t *buffer_id)
{
   int res, cmd_size;
   struct vtest_hdr cmd;
   struct vtest_payload_create_buffer *buffer;
   struct vtest_result result;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_BUFFER, sizeof(cmd));

   cmd_size = sizeof(*buffer) + sizeof(uint32_t) * info->queueFamilyIndexCount;
   buffer = alloca(cmd_size);
   buffer->device_id = device_id;
   buffer->flags = info->flags;
   buffer->device_size = info->size;
   buffer->usage_flags = info->usage;
   buffer->sharing_mode = info->sharingMode;
   buffer->queue_family_index_count = info->queueFamilyIndexCount;
   memcpy(buffer + 1, info->pQueueFamilyIndices, cmd_size - sizeof(*buffer));

   res = virgl_block_write(sock_fd, buffer, cmd_size);
   CHECK_IO_RESULT(res, cmd_size);

   /* reading the object ID allocated by virgl (or the error code) */
   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   *buffer_id = result.result;
   RETURN(result.error_code);
}

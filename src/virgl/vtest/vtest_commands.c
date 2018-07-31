#include <string.h>
#include <vulkan/vulkan.h>

#include "virgl_vtest.h"
#include "vtest_protocol.h"
#include "vtest_commands.h"

int vtest_create_command_pool(int sock_fd,
                              const struct vtest_command_pool_create_info *info,
                              uint32_t *handle)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_COMMAND_POOL, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   res = virgl_block_write(sock_fd, info, sizeof(*info));
   CHECK_IO_RESULT(res, sizeof(*info));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   *handle = result.result;

   return result.error_code;
}

int vtest_allocate_command_buffers(int sock_fd,
                                   const struct vtest_command_buffer_allocate_info *info,
                                   uint32_t *handles)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;

   INITIALIZE_HDR(cmd, VCMD_VK_ALLOCATE_COMMAND_BUFFERS, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   res = virgl_block_write(sock_fd, info, sizeof(*info));
   CHECK_IO_RESULT(res, sizeof(*info));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   res = virgl_block_read(sock_fd, handles, sizeof(uint32_t) * result.result);
   CHECK_IO_RESULT(res, sizeof(uint32_t) * result.result);

   return result.error_code;
}

int vtest_record_command(int sock_fd,
                         const struct vtest_command_record_info *info,
                         const uint32_t *descriptor_handles)
{

   ssize_t res;
   struct vtest_result result;
   struct vtest_hdr cmd;

   INITIALIZE_HDR(cmd, VCMD_VK_RECORD_COMMAND, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   res = virgl_block_write(sock_fd, info, sizeof(*info));
   CHECK_IO_RESULT(res, sizeof(*info));
   res = virgl_block_write(sock_fd,
                           descriptor_handles,
                           sizeof(uint32_t) * info->descriptor_count);
   CHECK_IO_RESULT(res, sizeof(uint32_t) * info->descriptor_count);

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   return result.error_code;
}

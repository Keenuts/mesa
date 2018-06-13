#include <stdint.h>
#include <stdio.h>

#include <vulkan/vulkan.h>
#include "common/macros.h"
#include "virgl_vtest.h"
#include "vtest_protocol.h"

#define CHECK_IO_RESULT(Done, Expected)                              \
   if ((Done) != (Expected)) {                                       \
      fprintf(stderr, "vtest IO failed. expected 0x%zx got 0x%zx\n", \
              (uint64_t)(Expected), (uint64_t)(Done));               \
      RETURN(-1);                                                    \
   }

#define CHECK_VTEST_RESULT(Result)                                            \
   if (Result.error_code != 0) {                                              \
      fprintf(stderr, "vtest returned an error: %d\n", result.error_code);    \
      RETURN(-2);                                                             \
   }


int vtest_send_create_device(int sock_fd)
{
   TRACE_IN();

   struct vtest_hdr cmd;
   struct vtest_result result;
   int res;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DEVICE, sizeof(cmd));

   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   CHECK_VTEST_RESULT(result);

   RETURN(result.id);
}

int vtest_send_get_physical_device_count(int sock_fd, uint32_t *device_count)
{
   TRACE_IN();

   struct vtest_hdr cmd;
   struct vtest_result result;
   int res;

   INITIALIZE_HDR(cmd, VCMD_VK_ENUMERATE_PHYSICAL_DEVICES, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   if (res < 0) {
      RETURN(-1);
   }

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   if (res < 0) {
      RETURN(-1);
   }

   if (result.error_code != 0) {
      fprintf(stderr, "vtest returned an error: %d\n", result.error_code);
      RETURN(-1);
   }

   *device_count = result.id;
   RETURN(0);
}

int vtest_send_get_sparse_properties(int sock_fd,
                                     int device_id,
                                     VkPhysicalDeviceSparseProperties *props)
{
   TRACE_IN();

   struct vtest_hdr cmd;
   struct vtest_result result;
   struct vtest_payload_device_get payload;
   int res;

   INITIALIZE_HDR(cmd, VCMD_VK_GET_PHYSICAL_DEVICE_SPARCE_PROPERTIES, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_id = device_id;
   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   CHECK_VTEST_RESULT(result);

   res = virgl_block_read(sock_fd, props, sizeof(*props));
   CHECK_IO_RESULT(res, sizeof(*props));

   RETURN(0);
}

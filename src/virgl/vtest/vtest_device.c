#include <stdint.h>
#include <stdio.h>

#include <vulkan/vulkan.h>
#include "common/macros.h"
#include "virgl_vtest.h"
#include "vtest_protocol.h"

int vtest_send_create_device(int sock_fd)
{
   TRACE_IN();

   struct vtest_hdr cmd;
   struct vtest_result result;
   int res;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DEVICE, sizeof(cmd));

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
   }

   RETURN(result.id);
}

int vtest_send_enumerate_physical_devices(int sock_fd, uint32_t *device_count)
{
   TRACE_IN();

   struct vtest_hdr cmd;
   struct vtest_result result;
   int res;

   INITIALIZE_HDR(cmd, VCMD_VK_ENUMERATE_PHYSICAL_DEVICES, sizeof(cmd));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   if (res < 0) {
      RETURN(-1);
   }

   if (result.error_code != 0) {
      fprintf(stderr, "vtest returned an error: %d\n", result.error_code);
   }

   *device_count = result.id;
   RETURN(0);
}

int vtest_send_get_sparse_properties(int sock_fd,
                                     int device_id,
                                     VkPhysicalDeviceSparseProperties *props)
{
   TRACE_IN();

   UNUSED_PARAMETER(sock_fd);
   UNUSED_PARAMETER(device_id);
   UNUSED_PARAMETER(props);

   RETURN(0);
}

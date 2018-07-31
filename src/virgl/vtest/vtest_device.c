#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vulkan/vulkan.h>
#include "virgl_vtest.h"
#include "vtest_protocol.h"

int vtest_get_physical_device_count(int sock_fd, uint32_t *device_count)
{

   struct vtest_hdr cmd;
   struct vtest_result result;
   ssize_t res;

   INITIALIZE_HDR(cmd, VCMD_VK_ENUMERATE_PHYSICAL_DEVICES, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   if (res < 0) {
      return -1;
   }

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   if (res < 0) {
      return -1;
   }

   if (result.error_code != 0) {
      fprintf(stderr, "vtest returned an error: %d\n", result.error_code);
      return -1;
   }

   *device_count = result.result;
   return 0;
}

int vtest_get_sparse_properties(int sock_fd,
                                int device_id,
                                VkPhysicalDeviceSparseProperties *props)
{
   struct vtest_hdr cmd;
   struct vtest_result result;
   struct vtest_payload_device_get payload;
   ssize_t res;


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

   return 0;
}

int vtest_get_queue_family_properties(int sock_fd,
                                      int device_id,
                                      uint32_t *family_count,
                                      VkQueueFamilyProperties **families)
{
   ssize_t res;
   struct vtest_hdr cmd;
   struct vtest_payload_device_get payload;
   struct vtest_result result;
   const uint32_t elt_size = sizeof(VkQueueFamilyProperties);


   INITIALIZE_HDR(cmd, VCMD_VK_GET_QUEUE_FAMILY_PROPS, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_id = device_id;
   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   CHECK_VTEST_RESULT(result);

   *family_count = result.result;
   *families = malloc(elt_size * result.result);
   if (*families == NULL) {
      return -1;
   }

   res = virgl_block_read(sock_fd, *families, elt_size * result.result);
   CHECK_IO_RESULT(res, elt_size * result.result);

   return 0;
}

int vtest_get_device_memory_properties(int sock_fd,
                                       int device_id,
                                       VkPhysicalDeviceMemoryProperties *props)
{
   ssize_t res;
   struct vtest_hdr cmd;
   struct vtest_payload_device_get payload;
   struct vtest_result result;


   INITIALIZE_HDR(cmd, VCMD_VK_GET_DEVICE_MEMORY, sizeof(cmd));
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

   return 0;
}

static int fill_queue_create_payload(struct vtest_payload_queue_create *payload,
                                     const VkDeviceQueueCreateInfo *info)
{
   const uint32_t array_size = sizeof(float) * info->queueCount;

   payload->flags = info->flags;
   payload->queue_family_index = info->queueFamilyIndex;
   payload->queue_count = info->queueCount;

   memcpy(payload + 1, info->pQueuePriorities, array_size);
   return 0;
}

int vtest_create_device(int sock_fd,
                        uint32_t physical_device_id,
                        const VkDeviceCreateInfo *info,
                        uint32_t *id)
{
   struct vtest_hdr cmd;
   struct vtest_payload_device_create payload;
   struct vtest_payload_queue_create *queue_info = NULL;
   struct vtest_result result;
   size_t payload_size;
   ssize_t res;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DEVICE, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   /* vtest_create_device payload is in two parts:
    *    - fixed size VkDeviceCreateInfo
    *    - variable size payload containing all VkDeviceQueueCreateInfos
    */
   memset(&payload, 0, sizeof(payload));
   payload.physical_device_id = physical_device_id;
   payload.flags = info->flags;
   payload.queue_info_count = info->queueCreateInfoCount;

   /* pEnabledFeatures CAN be NULL */
   if (info->pEnabledFeatures) {
      memcpy(&payload.features, info->pEnabledFeatures, sizeof(payload.features));
   }

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   /* For each VkDeviceQueueCreateInfo, we need to fill a queue payload */

   for (uint32_t i = 0; i < payload.queue_info_count; i++) {
      queue_info = alloca(sizeof(float) * info->pQueueCreateInfos[i].queueCount);
      res = fill_queue_create_payload(queue_info, info->pQueueCreateInfos + i);
      if (res < 0) {
         break;
      }

      payload_size = sizeof(*queue_info) + sizeof(float) * queue_info->queue_count;
      res = virgl_block_write(sock_fd, queue_info, payload_size);
      CHECK_IO_RESULT(res, payload_size);
   }

   /* reading the device ID allocated by virgl (or the error code) */
   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   *id = result.result;
   return result.result;
}

int vtest_destroy_device(int sock_fd,
                         uint32_t device_id)
{
   struct vtest_hdr cmd;
   struct vtest_payload_device_destroy payload;
   struct vtest_result result;
   ssize_t res;

   INITIALIZE_HDR(cmd, VCMD_VK_DESTROY_DEVICE, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_id = device_id;

   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   return result.result;
}

int vtest_read_memory(uint32_t sock_fd,
                      uint32_t device_handle,
                      uint32_t memory_handle,
                      uint64_t offset,
                      uint64_t size,
                      void *ptr)
{

   int res;
   struct vtest_hdr cmd;
   struct vtest_result result;
   struct vtest_payload_rw_memory payload;

   INITIALIZE_HDR(cmd, VCMD_VK_READ_MEMORY, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));


   payload.device_handle = device_handle;
   payload.memory_handle = memory_handle;
   payload.offset = offset;
   payload.size = size;

   res=virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));

   res=virgl_block_read(sock_fd, (char*)ptr, size);
   CHECK_IO_RESULT(res, size);

   return 0;
}

int vtest_write_memory(uint32_t sock_fd,
                       uint32_t device_handle,
                       uint32_t memory_handle,
                       uint64_t offset,
                       uint64_t size,
                       void *ptr)
{

   int res;
   struct vtest_hdr cmd;
   struct vtest_result result;
   struct vtest_payload_rw_memory payload;

   INITIALIZE_HDR(cmd, VCMD_VK_WRITE_MEMORY, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));


   payload.device_handle = device_handle;
   payload.memory_handle = memory_handle;
   payload.offset = offset;
   payload.size = size;

   res=virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res=virgl_block_write(sock_fd, (char*)ptr + offset, size);
   CHECK_IO_RESULT(res, size);

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   return result.result;
   return 0;
}

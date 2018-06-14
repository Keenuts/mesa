#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


int vtest_get_physical_device_count(int sock_fd, uint32_t *device_count)
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

   *device_count = result.result;
   RETURN(0);
}

int vtest_get_sparse_properties(int sock_fd,
                                int device_id,
                                VkPhysicalDeviceSparseProperties *props)
{
   struct vtest_hdr cmd;
   struct vtest_result result;
   struct vtest_payload_device_get payload;
   int res;

   TRACE_IN();

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

int vtest_get_queue_family_properties(int sock_fd,
                                      int device_id,
                                      uint32_t *family_count,
                                      VkQueueFamilyProperties **families)
{
   int res;
   struct vtest_hdr cmd;
   struct vtest_payload_device_get payload;
   struct vtest_result result;

   TRACE_IN();

   INITIALIZE_HDR(cmd, VCMD_VK_GET_QUEUE_FAMILY_PROPS, sizeof(cmd));
   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));

   payload.device_id = device_id;
   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   CHECK_VTEST_RESULT(result);

   uint64_t size = sizeof(**families) * result.result;
   *family_count = result.result;
   *families = malloc(size);
   if (*families == NULL) {
      RETURN(-1);
   }

   res = virgl_block_read(sock_fd, *families, size);
   CHECK_IO_RESULT(res, (int)size);

   RETURN(0);
}

int vtest_create_device(int sock_fd,
                        uint32_t physical_device_id,
                        const VkDeviceCreateInfo *info,
                        uint32_t *id)
{
   UNUSED_PARAMETER(id);
   UNUSED_PARAMETER(info);

   struct vtest_hdr cmd;
   struct vtest_payload_device_create payload;
   struct vtest_result result;
   int res;

   INITIALIZE_HDR(cmd, VCMD_VK_CREATE_DEVICE, sizeof(cmd));

   payload.physical_device_id = physical_device_id;
   payload.flags = info->flags;
   payload.queue_info_count = info->queueCreateInfoCount;
   memcpy(&payload.features, info->pEnabledFeatures, sizeof(payload.features));


   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));
   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));

   for (uint32_t i = 0; i < payload.queue_info_count; i++) {
      struct vtest_payload_queue_create *queue_info = NULL;
      uint32_t queue_count = info->pQueueCreateInfos->queueCount;

      queue_info = malloc(sizeof(queue_info) + sizeof(float) * queue_count);
      if (queue_info == NULL) {
         RETURN(-1);
      }

      queue_info->flags = info->pQueueCreateInfos[i].flags;
      queue_info->queue_family_index = info->pQueueCreateInfos[i].queueFamilyIndex;
      queue_info->queue_count = info->pQueueCreateInfos[i].queueCount;

      memcpy(&queue_info->priorities,
             info->pQueueCreateInfos[i].pQueuePriorities,
             sizeof(float) * queue_count);
   }


   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   CHECK_VTEST_RESULT(result);

   *id = result.result;

   RETURN(result.result);
}

int vtest_get_device_queues(int sock_fd,
                            uint32_t device_id,
                            uint32_t *queue_count,
                            struct vk_queue **queues)
{
   TRACE_IN();

   struct vtest_hdr cmd;
   struct vtest_payload_device_get payload;
   struct vtest_result result;
   struct vtest_payload_queue_get queue_result;
   int res;

   INITIALIZE_HDR(cmd, VCMD_VK_GET_DEVICE_QUEUES, sizeof(cmd));
   payload.device_id = device_id;

   res = virgl_block_write(sock_fd, &cmd, sizeof(cmd));
   CHECK_IO_RESULT(res, sizeof(cmd));
   res = virgl_block_write(sock_fd, &payload, sizeof(payload));
   CHECK_IO_RESULT(res, sizeof(payload));


   res = virgl_block_read(sock_fd, &result, sizeof(result));
   CHECK_IO_RESULT(res, sizeof(result));
   CHECK_VTEST_RESULT(result);

   *queue_count = result.result;
   *queues = malloc(sizeof(struct vk_queue) * result.result);
   if (*queues == NULL) {
      RETURN(-1);
   }

   for (uint32_t i = 0; i < result.result; i++) {
      res = virgl_block_read(sock_fd, &queue_result, sizeof(queue_result));
      CHECK_IO_RESULT(res, sizeof(queue_result));

      (*queues)[i].identifier = queue_result.identifier;
      (*queues)[i].queue_index = queue_result.queue_index;
      (*queues)[i].family_index = queue_result.family_index;
   }

   RETURN(0);
}

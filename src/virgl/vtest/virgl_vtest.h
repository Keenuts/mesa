#ifndef VIRGL_VTEST_H
#define VIRGL_VTEST_H

#include <stdio.h>
#include <vulkan/vulkan.h>
#include "vulkan/vk_structs.h"

#define INITIALIZE_HDR(Cmd, Id, Size)                          \
    do {                                                       \
        Cmd.length = Size / sizeof(uint32_t);                  \
        Cmd.id = Id;                                           \
    } while (0);

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

int virgl_block_write(int fd, const void *buf, int size);
int virgl_block_read(int fd, void *buf, int size);

int virgl_vtest_connect(void);

int vtest_get_physical_device_count(int sock_fd, uint32_t *device_count);
int vtest_get_sparse_properties(int sock_fd,
                                     int device_id,
                                     VkPhysicalDeviceSparseProperties *props);
int vtest_get_queue_family_properties(int sock_fd,
                                      int device_id,
                                      uint32_t *family_count,
                                      VkQueueFamilyProperties **families);

int vtest_create_device(int sock_fd,
                        uint32_t physical_device_id,
                        const VkDeviceCreateInfo *info,
                        uint32_t *id);
int vtest_get_device_queues(int sock_fd,
                            uint32_t device_id,
                            uint32_t *queue_count,
                            struct vk_queue **queues);

int vtest_create_descriptor_layout(int sock_fd,
                                   uint32_t device_id,
                                   const VkDescriptorSetLayoutCreateInfo *info,
                                   uint32_t *descriptor_set_id);

int vtest_create_buffer(int sock_fd,
                        uint32_t device_id,
                        const VkBufferCreateInfo *info,
                        uint32_t *buffer_id);

#endif

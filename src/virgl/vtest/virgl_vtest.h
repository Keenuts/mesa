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
   if ((Done) < 0 || (size_t)(Done) != (Expected)) {                 \
      fprintf(stderr, "vtest IO failed. expected 0x%zx got 0x%zx\n", \
              (uint64_t)(Expected), (uint64_t)(Done));               \
      return -1;                                                    \
   }

#define CHECK_VTEST_RESULT(Result)                                            \
   if (Result.error_code != 0) {                                              \
      fprintf(stderr, "vtest returned an error: %d\n", result.error_code);    \
      return -2;                                                             \
   }

ssize_t virgl_block_write(int fd, const void *ptr, size_t size);
ssize_t virgl_block_read(int fd, void *ptr, size_t size);

int virgl_vtest_connect(void);

int vtest_get_physical_device_count(int sock_fd, uint32_t *device_count);
int vtest_get_sparse_properties(int sock_fd,
                                     int device_id,
                                     VkPhysicalDeviceSparseProperties *props);
int vtest_get_queue_family_properties(int sock_fd,
                                      int device_id,
                                      uint32_t *family_count,
                                      VkQueueFamilyProperties **families);

int vtest_get_device_memory_properties(int sock_fd,
                                       int device_id,
                                       VkPhysicalDeviceMemoryProperties *props);

int vtest_create_device(int sock_fd,
                        uint32_t physical_device_id,
                        const VkDeviceCreateInfo *info,
                        uint32_t *id);

int vtest_destroy_device(int sock_fd,
                         uint32_t device_id);

int vtest_get_device_queues(int sock_fd,
                            uint32_t device_id,
                            uint32_t *queue_count,
                            struct vk_queue **queues);

int vtest_read_memory(uint32_t sock_fd,
                      uint32_t device_handle,
                      uint32_t memory_handle,
                      uint64_t offset,
                      uint64_t size,
                      void *ptr);

int vtest_write_memory(uint32_t sock_fd,
                       uint32_t device_handle,
                       uint32_t memory_handle,
                       uint64_t offset,
                       uint64_t size,
                       void *ptr);
#endif

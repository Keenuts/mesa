#ifndef VIRGL_VTEST_H
#define VIRGL_VTEST_H

#include <vulkan/vulkan.h>

struct vtest_hdr {
    union {
        uint32_t raw[2];
        struct {
            uint32_t length;
            uint32_t id;
        };
    };
};

struct vtest_result {
    uint32_t error_code;
    uint32_t result;
};

#define INITIALIZE_HDR(Cmd, Id, Size)                          \
    do {                                                       \
        Cmd.length = Size / sizeof(uint32_t);                  \
        Cmd.id = Id;                                           \
    } while (0);

int virgl_block_write(int fd, const void *buf, int size);
int virgl_block_read(int fd, void *buf, int size);

int virgl_vtest_connect(void);

int vtest_create_device(int sock_fd);
int vtest_get_physical_device_count(int sock_fd, uint32_t *device_count);
int vtest_get_sparse_properties(int sock_fd,
                                     int device_id,
                                     VkPhysicalDeviceSparseProperties *props);
int vtest_get_queue_family_properties(int sock_fd,
                                      int device_id,
                                      uint32_t *family_count,
                                      VkQueueFamilyProperties **families);

#endif

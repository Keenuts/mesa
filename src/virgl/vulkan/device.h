#ifndef DEVICE_H
#define DEVICE_H

struct vtest_device
{
   uint32_t connected;
   int sock_fd;
};

extern struct vtest_device vtest_device;

#endif

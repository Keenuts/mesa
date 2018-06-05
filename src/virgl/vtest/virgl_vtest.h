#ifndef VIRGL_VTEST_H
#define VIRGL_VTEST_H

struct virgl_vtest {
    int sock_fd;
};

int virgl_block_write(int fd, void *buf, int size);
int virgl_block_read(int fd, void *buf, int size);

int virgl_vtest_connect(struct virgl_vtest *vtest);

#endif

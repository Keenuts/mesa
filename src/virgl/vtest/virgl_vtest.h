#ifndef VIRGL_VTEST_H
#define VIRGL_VTEST_H

int virgl_block_write(int fd, const void *buf, int size);
int virgl_block_read(int fd, void *buf, int size);

int virgl_vtest_connect(void);

#endif

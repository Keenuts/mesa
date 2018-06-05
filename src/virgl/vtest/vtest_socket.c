/*
 * Copyright 2014, 2015 Red Hat.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>

#include <os/os_process.h>
#include <util/u_format.h>
/* connect to remote socket */
#define VTEST_SOCKET_NAME "/tmp/.virgl_test"

#include "vtest_protocol.h"
#include "virgl_vtest.h"

/* block read/write routines */
int virgl_block_write(int fd, void *buf, int size)
{
   void *ptr = buf;
   int left;
   int ret;
   left = size;
   do {
      ret = write(fd, ptr, left);
      if (ret < 0)
         return -errno;
      left -= ret;
      ptr += ret;
   } while (left);
   return size;
}

int virgl_block_read(int fd, void *buf, int size)
{
   void *ptr = buf;
   int left;
   int ret;
   left = size;
   do {
      ret = read(fd, ptr, left);
      if (ret <= 0) {
         fprintf(stderr,
                 "lost connection to rendering server on %d read %d %d\n",
                 size, ret, errno);
         abort();
         return ret < 0 ? -errno : 0;
      }
      left -= ret;
      ptr += ret;
   } while (left);
   return size;
}

static int virgl_vtest_send_init(struct virgl_vtest *vtest)
{
   uint32_t buf[VTEST_HDR_SIZE];
   const char *nstr = "virtest";
   char cmdline[64];
   int ret;

   ret = os_get_process_name(cmdline, 63);
   if (ret == FALSE)
      strcpy(cmdline, nstr);
#if defined(__GLIBC__) || defined(__CYGWIN__)
   if (!strcmp(cmdline, "shader_runner")) {
      const char *name;
      /* hack to get better testname */
      name = program_invocation_short_name;
      name += strlen(name) + 1;
      strncpy(cmdline, name, 63);
   }
#endif
   buf[VTEST_CMD_LEN] = strlen(cmdline) + 1;
   buf[VTEST_CMD_ID] = VCMD_CREATE_RENDERER;

   virgl_block_write(vtest->sock_fd, &buf, sizeof(buf));
   virgl_block_write(vtest->sock_fd, (void *)cmdline, strlen(cmdline) + 1);
   return 0;
}

int virgl_vtest_connect(struct virgl_vtest *vtest)
{
   struct sockaddr_un un;
   int sock, ret;

   sock = socket(PF_UNIX, SOCK_STREAM, 0);
   if (sock < 0)
      return -1;

   memset(&un, 0, sizeof(un));
   un.sun_family = AF_UNIX;
   snprintf(un.sun_path, sizeof(un.sun_path), "%s", VTEST_SOCKET_NAME);

   do {
      ret = 0;
      if (connect(sock, (struct sockaddr *)&un, sizeof(un)) < 0) {
         ret = -errno;
      }
   } while (ret == -EINTR);

   vtest->sock_fd = sock;
   virgl_vtest_send_init(vtest);
   return 0;
}

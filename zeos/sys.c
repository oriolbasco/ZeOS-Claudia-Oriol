/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_write(void * st)
{
  if (!access_ok(VERIFY_READ, st, 3 * sizeof(unsigned int))) return -EFAULT;

  unsigned int args[3];
  if (copy_from_user(st, args, 3 * sizeof(unsigned int)) < 0) return -EFAULT;

  int fd = (int)args[0];
  char *buffer = (char *)args[1];
  int size = (int)args[2];

  int err = check_fd(fd, ESCRIPTURA);
  if (err < 0) return err;

  if (buffer == NULL || size < 0) return -EINVAL;
  if (size == 0) return 0;

  if (!access_ok(VERIFY_READ, buffer, size)) return -EFAULT;

  int sizeAux = size;
  int bytesEscrits = 0;
  char local_buff[256];

  while (sizeAux > 0)
  {
    int sizeChunk;
    if (sizeAux > 256) sizeChunk = 256;
    else sizeChunk = sizeAux;

    if (copy_from_user(buffer, local_buff, sizeChunk) < 0) return -EFAULT;

    int ret = sys_write_console(local_buff, sizeChunk);
    if (ret < 0) return ret;

    sizeAux -= ret;
    bytesEscrits += ret;
    buffer += ret;
  }

  return bytesEscrits;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

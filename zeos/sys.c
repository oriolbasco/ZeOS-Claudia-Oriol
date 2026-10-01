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

int sys_write(unsigned int * st)
{
  if (!access_ok(VERIFY_READ, st, 3 * sizeof(unsigned int))) return -EFAULT;

  unsigned int args[3];
  if (copy_from_user(st, args, 3 * sizeof(unsigned int)) < 0) return -EFAULT;

  int fd = (int)args[0];
  char *buffer = (char *)args[1];
  int size = (int)args[2];

  int err = check_fd(fd, ESCRIPTURA);
  if (err < 0) return err;

  if (buffer == NULL || size < 0 || size > 256) return -EINVAL;

  if (!access_ok(VERIFY_READ, buffer, size)) return -EFAULT;

  char local_buff[256];
  if (copy_from_user(buffer, local_buff, size) < 0) return -EFAULT;

  return sys_write_console(local_buff, size);
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

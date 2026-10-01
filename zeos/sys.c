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

int sys_write(int fd, char * buffer, int size)
{
  if (check_fd(fd, ESCRIPTURA) < 0) return -EBADF; // error en el pas de parametres

  if (buffer == NULL || size < 0 || size > 256) return -EINVAL;

  char buff[256];

  if (!access_ok(VERIFY_READ, buffer, size)) return -EFAULT;

  if (copy_from_user(buffer, buff, size) < 0) return -EFAULT; // error en la copia del buffer d espai d usuari a espai de sistema
  
  return sys_write_console(buff, size);
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

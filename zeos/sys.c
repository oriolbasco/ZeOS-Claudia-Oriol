/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}


int sys_write(int fd, void* buffer, int size) {
  //check parametres:
  if(check_fd(fd,ESCRIPTURA) != 0) return -1;
  else if(buffer == NULL) return -1;
  else if(size <= 0) return -1;
  //aqui hem de utlilitzar copy_from_user???
  
  return sys_write_console(buffer, size); 
}

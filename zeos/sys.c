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

//acces_okei --> para ver si un puntero es correcto(nos viene del usuario); para el puntero 
//buffer pequeño para ir haciendo copy_from_user???
int sys_write(void *parmetres) {
  //check parametres:
  //if(check_fd(fd,ESCRIPTURA) != 0) return -1;
  //else if(buffer == NULL) return -1;
  //else if(size <= 0) return -1;
  //aqui hem de utlilitzar copy_from_user???
  
  int fd;
  char *buffer;
  int size;

  fd = *((int *)parmetres);
  buffer = *((char **)(parmetres + sizeof(int)));
  size = *((int *)(parmetres + 2*sizeof(int)));
  
  
  return sys_write_console(buffer, size);
}

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
#define chunk 256

extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF;
  if (permissions!=ESCRIPTURA) return -EACCES;
  return 0;
}

/**
 * @name sys_write
 * @param st punter void, apunta al primer parametre de la funcio write guardat a la pila d'usuari
 * @return si tot esta be retorna el numero de bytes escrits, sino retorna un error code. 
 */
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
  char local_buff[chunk];

  while (sizeAux > 0)
  {
    int sizeChunk;
    if (sizeAux > chunk) sizeChunk = chunk;
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

int sys_gettime()
{
  return zeos_ticks;
}

int sys_getpid()
{
  return current()->PID;
}

int sys_fork() {
  int PCB = alloc_frame();
	union task_union *taskFill_union = (union task_union *) (PCB << 12); //nou espai fisic del fill
  set_ss_pag(get_PT(current()), PCB, PCB, 0); //afegim al page table del pare el frame del fill
  //copio espai de memoria del pare al fill
  copy_data(current(), taskFill_union, PAGE_SIZE);
}

int sys_ni_syscall()
{
	return -ENOSYS;
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

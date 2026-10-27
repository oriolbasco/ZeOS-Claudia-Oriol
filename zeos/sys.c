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
extern void ret_from_fork(void);

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
  struct task_struct *parent = current(); //PCB del pare
  page_table_entry *system_pt = get_PT(parent); //punter a la taula de pagines de sistema del pare
  int pcb_frame = alloc_frame(); //frame per al pcb del fill
  if (pcb_frame == -1) return -EAGAIN; //no hi ha

  set_ss_pag(system_pt, pcb_frame, pcb_frame, 0); //nova entrada a la TP del sistema

  union task_union *child = (union task_union *)((unsigned int)pcb_frame << 12);
  copy_data(parent, child, PAGE_SIZE); //fill hereda el context del pare
  struct task_struct *child_pcb = &child->task;

  int dir_frame = alloc_frame(); //frame per al directori de pagines del fill
  if (dir_frame == -1) return -EAGAIN; //no hi ha
  set_ss_pag(system_pt, dir_frame, dir_frame, 0); //nova entrada a la TP del sistema

  int user_pt_frame = alloc_frame(); //frame per a la taula de pagines d'usuari del fill
  if (user_pt_frame == -1) return -EAGAIN; //no hi
  set_ss_pag(system_pt, user_pt_frame, user_pt_frame, 0); //nova entrada a la TP del sistema

  page_table_entry *child_dir = (page_table_entry *)((unsigned int)dir_frame << 12);
  page_table_entry *child_user_pt = (page_table_entry *)((unsigned int)user_pt_frame << 12);
  clear_page_table(child_dir);
  clear_page_table(child_user_pt);

  //entrada 0: TP del sistema
  child_dir[0] = get_DIR(parent)[0];

  // Entrada 1: taula d'usuari nova del fill
  set_ss_pag(child_dir, 1, user_pt_frame, 1);

  child_pcb->dir_pages_baseAddr = child_dir;

  page_table_entry *parent_user_pt = (page_table_entry *)((unsigned int)get_DIR(parent)[1].bits.pbase_addr << 12);

  // Copiem les entrades de codi del pare al fill, compartir codi
  copy_data(&parent_user_pt[NUM_PAG_DATA], &child_user_pt[NUM_PAG_DATA], NUM_PAG_CODE * sizeof(page_table_entry));

  //copiem pila+dades d'usuari en nous frames de memoria
  const unsigned int tmp = NUM_PAG_DATA + NUM_PAG_CODE; //entrada lliure 8+20
  for (int i = 0; i < NUM_PAG_DATA; i++) {
    if (parent_user_pt[i].bits.present == 1) {
      int frame = alloc_frame(); //reservem un frame del fill
      if (frame == -1) return -EAGAIN; //no hi ha
      set_ss_pag(child_user_pt, i, frame, 1); //nova entrada a la TP d'usuari del fill

      //La dirección temporal del padre apunta al frame del hijo
      set_ss_pag(parent_user_pt, tmp, frame, 0);

      //copiem una pagina de dades del pare al fill
      copy_data((void *)(L_USER_START + i * PAGE_SIZE), (void *)(L_USER_START + tmp * PAGE_SIZE), PAGE_SIZE);

      del_ss_pag(parent_user_pt, tmp); //eliminem entrada temporal del pare
      set_cr3(get_DIR(parent)); //buidem TLB
      //reultilitzem la mateixa entrada temporal per a les 20 pagines.
    }
  }

  // Valor que rebra el fill com a resultat del fork. EAX=0; 
  child->stack[KERNEL_STACK_SIZE - 10] = 0;

  // EBP inicial fals(consumit pel pop %ebp)
  child->stack[KERNEL_STACK_SIZE - 18] = 0;
  //direccio de ret_from_fork
  child->stack[KERNEL_STACK_SIZE - 17] = (unsigned long)ret_from_fork;

  child_pcb->kernel_esp = (unsigned int)&child->stack[KERNEL_STACK_SIZE - 18];

  child_pcb->PID = assign_PID(); //assignem un nou PID al fill

  INIT_LIST_HEAD(&(child_pcb->list)); //inicialitzem la llista del fill
  list_add_tail(&(child_pcb->list), &ready_queue); //afegim
  return child_pcb->PID; //retornem el PID del fill
}

int sys_ni_syscall()
{
	return -ENOSYS;
}

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
#include <buffer_circular.h>

#define LECTURA 0
#define ESCRIPTURA 1
#define chunk 256

extern int zeos_ticks;
extern void ret_from_fork(void);
extern void clock_handler_end(void);

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

int sys_fork() 
{
  struct task_struct *parent_pcb = current(); //PCB del pare
  page_table_entry *SPTAddress = get_PT(parent_pcb); //punter a la taula de pagines de sistema del pare
  int PCBFill = alloc_frame(); //frame per al pcb del fill
  if (PCBFill == -1) return -EAGAIN; //no hi ha

  set_ss_pag(SPTAddress, PCBFill, PCBFill, 0); //nova entrada a la TP del sistema

  union task_union *child_union = (union task_union *)((unsigned int)PCBFill << 12);
  copy_data(parent_pcb, child_union, PAGE_SIZE); //fill hereda el context del pare
  struct task_struct *child_pcb = &child_union->task;

  int DirFill = alloc_frame(); //frame per al directori de pagines del fill
  if (DirFill == -1) return -EAGAIN; //no hi ha
  set_ss_pag(SPTAddress, DirFill, DirFill, 0); //nova entrada a la TP del sistema

  int UserPTFill = alloc_frame(); //frame per a la taula de pagines d'usuari del fill
  if (UserPTFill == -1) return -EAGAIN; //no hi ha
  set_ss_pag(SPTAddress, UserPTFill, UserPTFill, 0); //nova entrada a la TP del sistema

  page_table_entry *DirFillAddress = (page_table_entry *)((unsigned int)DirFill << 12);
  page_table_entry *UPTFillAddress = (page_table_entry *)((unsigned int)UserPTFill << 12);
  clear_page_table(DirFillAddress);
  clear_page_table(UPTFillAddress);

  //entrada 0: TP del sistema
  DirFillAddress[0] = get_DIR(parent_pcb)[0];

  // Entrada 1: taula d'usuari nova del fill
  set_ss_pag(DirFillAddress, 1, UserPTFill, 1);

  child_pcb->dir_pages_baseAddr = DirFillAddress;

  page_table_entry *UPTPareAddress = (page_table_entry *)((unsigned int)get_DIR(parent_pcb)[1].bits.pbase_addr << 12);

  // Copiem les entrades de codi del pare al fill, compartir codi
  copy_data(&UPTPareAddress[NUM_PAG_DATA], &UPTFillAddress[NUM_PAG_DATA], NUM_PAG_CODE * sizeof(page_table_entry));

  //copiem pila+dades d'usuari en nous frames de memoria
  const unsigned int tmp = NUM_PAG_DATA + NUM_PAG_CODE; //entrada lliure 8+20
  for (int i = 0; i < NUM_PAG_DATA; i++) 
  {
    if (UPTPareAddress[i].bits.present == 1) 
    {
      int frame = alloc_frame(); //reservem un frame del fill
      if (frame == -1) return -EAGAIN; //no hi ha
      set_ss_pag(UPTFillAddress, i, frame, 1); //nova entrada a la TP d'usuari del fill

      //La dirección temporal del padre apunta al frame del hijo
      set_ss_pag(UPTPareAddress, tmp, frame, 0);

      //copiem una pagina de dades del pare al fill
      copy_data((void *)(L_USER_START + i * PAGE_SIZE), (void *)(L_USER_START + tmp * PAGE_SIZE), PAGE_SIZE);

      del_ss_pag(UPTPareAddress, tmp); //eliminem entrada temporal del pare
      set_cr3(get_DIR(parent_pcb)); //buidem TLB
      //reultilitzem la mateixa entrada temporal per a les 20 pagines.
    }
  }

  //les regions de malloc s'hereden: el vector ja s'ha copiat amb el PCB, copiem les pagines
  for (int r = 0; r < MAX_REGIONS; r++)
  {
    for (unsigned int p = parent_pcb->regions[r].start_page;
         parent_pcb->regions[r].num_pages != 0 &&
         p < parent_pcb->regions[r].start_page + parent_pcb->regions[r].num_pages; p++)
    {
      int frame = alloc_frame();
      if (frame == -1) return -EAGAIN;
      set_ss_pag(UPTFillAddress, p, frame, 1);

      set_ss_pag(UPTPareAddress, tmp, frame, 0);
      copy_data((void *)(L_USER_START + p * PAGE_SIZE), (void *)(L_USER_START + tmp * PAGE_SIZE), PAGE_SIZE);
      del_ss_pag(UPTPareAddress, tmp);
      set_cr3(get_DIR(parent_pcb));
    }
  }

  // Valor que rebra el fill com a resultat del fork. EAX=0; 
  child_union->stack[KERNEL_STACK_SIZE - 10] = 0;
  // EBP inicial fals(consumit pel pop %ebp)
  child_union->stack[KERNEL_STACK_SIZE - 19] = 0;
  child_union->stack[KERNEL_STACK_SIZE - 18] = (unsigned long)ret_from_fork; //direccio de ret_from_fork
  child_union->stack[KERNEL_STACK_SIZE - 17] = (unsigned long)clock_handler_end; // on hi havia l'ebx parametre, el fill torna a usuari pel codi del clock_handler
  child_pcb->kernel_esp = (unsigned int)&child_union->stack[KERNEL_STACK_SIZE - 19];
  child_pcb->PID = assign_PID(); //assignem un nou PID al fill
  child_pcb->state = ST_READY;

  INIT_LIST_HEAD(&(child_pcb->list)); //inicialitzem la llista del fill

  update_process_state_rr(child_pcb, &ready_queue);
  return child_pcb->PID; //retornem el PID del fill
}

void sys_exit()
{
  struct task_struct *pcb = current();
  page_table_entry *dir = get_DIR(pcb);
  page_table_entry *UPTAddress = (page_table_entry *)((unsigned int)dir[1].bits.pbase_addr << 12); //TP d'usuari, entrada 1 del dir

  //alliberem dades+pila d'usuari, el codi es compartit i no s'allibera
  for (int i = 0; i < NUM_PAG_DATA; i++)
  {
    if (UPTAddress[i].bits.present == 1)
    {
      free_frame(UPTAddress[i].bits.pbase_addr);
      del_ss_pag(UPTAddress, i);
    }
  }

  //alliberem les regions de malloc que no s'han fet free
  for (int r = 0; r < MAX_REGIONS; r++)
  {
    for (unsigned int p = pcb->regions[r].start_page;
         pcb->regions[r].num_pages != 0 &&
         p < pcb->regions[r].start_page + pcb->regions[r].num_pages; p++)
    {
      free_frame(UPTAddress[p].bits.pbase_addr);
      del_ss_pag(UPTAddress, p);
    }
    pcb->regions[r].num_pages = 0;
  }

  //free_frame nomes els marca lliures, podem seguir fent servir la pila i el dir fins al task_switch
  free_frame((unsigned int)UPTAddress >> 12);
  free_frame((unsigned int)dir >> 12);
  free_frame((unsigned int)pcb >> 12);

  del_ss_pag(dir, (unsigned int)UPTAddress >> 12);

  sched_next_rr(); //no tornem mai aqui
}


int sys_read(void *st) 
{
  unsigned int args[2];

  if (!access_ok(VERIFY_READ, st, sizeof(args))) return -EFAULT;
  if (copy_from_user(st, args, sizeof(args)) < 0) return -EFAULT;

  char *b = (char *)args[0];
  int maxchars = (int)args[1];

  if (b == NULL || maxchars < 0) return -EINVAL;
  if (maxchars == 0) return 0;
  if (!access_ok(VERIFY_WRITE, b, maxchars)) return -EFAULT;

  struct task_struct *task = current();
  read_bytes = (unsigned int)maxchars; //guardem el num de bytes que necessita llegir el proces actual

  if (reader == NULL) reader = task;

  // esperem el torn
  if (reader != task) block_current(&blocked);

  int copied = 0;
  char c;

  // si maxchars > CBUF_SIZE no caben tots buidem el buffer per tandes
  while (copied < maxchars) {
    if (keyboard_buffer.count < read_bytes && keyboard_buffer.count < CBUF_SIZE) block_current(&blocked);
    while (copied < maxchars && cbuf_pop(&keyboard_buffer, &c)) // Copiem els bytes del buffer circular a l'espai d'usuari
    {
      copy_to_user(&c, b + copied, 1);
      ++copied;
      --read_bytes; // ara read_bytes son els bytes que encara falten
    }
  }

  read_bytes = 0; //reiniciem el num de bytes que necessita llegir el proces actual

  //no hi ha cap proces bloquejat
  if (list_empty(&blocked)) reader = NULL;

  //el seguent proces bloquejat es el seguent reader
  else 
  {
    reader = list_head_to_task_struct(list_first(&blocked));
    wake_reader();
  }

  return copied;
}

/*
  reserva una regio. Calcula quantitat de pagines necesita usuari.
  Busca pagines virtuals i lliures i consecutives en l'espai d'usuari
  Asigna i mapeja frames fisics
  Retorna direccio inicial
*/
char *sys_malloc(void *st) 
{
  int bytes_user;

  if (!access_ok(VERIFY_READ, st, sizeof(bytes_user))) return NULL;
  if (copy_from_user(st, &bytes_user, sizeof(bytes_user)) < 0) return NULL;
  if (bytes_user <= 0) return NULL;
  
  unsigned int pages = 1 + (((unsigned int)bytes_user + 4) - 1) / PAGE_SIZE;
  if (pages > TOTAL_PAGES - HEAP_FIRST_PAGE) return NULL;
  
  page_table_entry *dir = get_DIR(current()); //obtenim el directori del proces actual
  page_table_entry *pt = (page_table_entry *)((unsigned long)dir[1].bits.pbase_addr << 12); //obtenim la taula de pagines d'usuari del proces actual

  unsigned int first = 0;
  unsigned int consecutive = 0;

  //busquem pagines virtuals lliures i consecutives en l'espai d'usuari
  for (unsigned int page = HEAP_FIRST_PAGE; page < TOTAL_PAGES; ++page) 
  {
    if (pt[page].bits.present) consecutive = 0; //si esta ocupada, reiniciem el comptador de consecutives
    else if (++consecutive == pages) 
    {
      first = page + 1 - pages; //direccio de la primera pagina virtual lliure consecutiva
      break;
    }
  }

  if (first == 0) return NULL; //no hi ha espai suficient

  
  for (unsigned int page = first; page < first + pages; ++page) 
  {
    int frame = alloc_frame();

    //si no hi ha frames fisics lliures, alliberem els que ja haviem asignat i retornem NULL
    if (frame == -1) 
    { 
      while (page > first) 
      {
        --page;
        unsigned int allocated_frame = get_frame(pt, page);
        del_ss_pag(pt, page);
        free_frame(allocated_frame);
      }

      set_cr3(dir);
      return NULL; 
    }
    //assignem i mapejem el frame fisic a la pagina virtual
    set_ss_pag(pt, page, frame, 1);
  }

  set_cr3(dir);
  char *region = (char *)(L_USER_START + (unsigned long)first * PAGE_SIZE); //direccio inicial de la regio d'usuari

  //inicialitzem la regio a 0
  for (unsigned int i = 0; i < pages * PAGE_SIZE; ++i) region[i] = 0;
  
  *(int *)region = bytes_user; //capcalera: size de la regio als 4 primers bytes
    
  return region + 4;
}

/*free(char*regio)
  Busca regio en el registre del proces, punter ha de coincidir amb direccio inicial que retornat el malloc
  Amb el num de pagines guardat kernell elimina mapeijos, frames fisics i registre 
*/
int sys_free(void *st) {
  char *region;

  if (!access_ok(VERIFY_READ, st, sizeof(region))) return -EFAULT;
  if (copy_from_user(st, &region, sizeof(region)) < 0) return -EFAULT;

  unsigned long addr = (unsigned long)region;
  if (addr < L_USER_START) return -EINVAL;
  if (addr & (PAGE_SIZE - 1)) return -EINVAL; //ha de ser l'adreca inicial retornada per malloc
  unsigned int start = (addr - L_USER_START) >> 12;

  //busquem la regio al vector del proces
  struct heap_region *r = NULL;
  for (int i = 0; i < MAX_REGIONS; ++i) {
    struct heap_region *c = &current()->regions[i];
    if (c->num_pages != 0 && c->start_page == start) { r = c; break; }
  }
  if (r == NULL) return -EINVAL;

  page_table_entry *dir = get_DIR(current());
  page_table_entry *pt = (page_table_entry *)((unsigned long)dir[1].bits.pbase_addr << 12);

  for (unsigned int p = r->start_page; p < r->start_page + r->num_pages; ++p) {
    free_frame(get_frame(pt, p));
    del_ss_pag(pt, p);
  }
  r->num_pages = 0; //entrada lliure
  set_cr3(dir); //buidem TLB
  return 0;
}

int sys_create_thread(void* st)
{
  
}

int sys_ni_syscall()
{
	return -ENOSYS;
}
/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>

struct list_head ready_queue;
struct task_struct *init_task;
struct task_struct *idle_task;
int ticks_restants;

char initial_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned(4096))); // Space for the initial system stack
static int next_pid = 2;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t)
{
       return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t)
{
       return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}

void cpu_idle(void)
{
	__sti();
	while(1)
	{
	;
	}
}

void init_idle (void)
{
	int Dir = alloc_frame(); // reservem un frame per el nou dir
	page_table_entry *DirAddress = (page_table_entry *) (Dir << 12); // definim un punter al dir d'idle
	page_table_entry *SPTAddress = get_PT(init_task); // nou punter a la taula de pagines de sistema
	set_ss_pag(SPTAddress, Dir, Dir, 0); // nova entrada a la taula de pagines de sistema amb dirIdle -> dirIdle
	clear_page_table(DirAddress);
	set_ss_pag(DirAddress, 0, ((unsigned int) SPTAddress) >> 12, 0); // nova entrada a dirIdle que asigna entrada 0 del dir amb la taula de pagines de sistema

	int PCB = alloc_frame(); // reservem espai pel pcb
	set_ss_pag(SPTAddress, PCB, PCB, 0); // creem una entrada a la taula de pagines de sistema per pcb -> pcb
	union task_union *idle_union = (union task_union *) (PCB << 12); // associem a aquest espai el union
	struct task_struct *idle_pcb = &(idle_union->task);

	idle_pcb->PID = 0;
    idle_pcb->dir_pages_baseAddr = DirAddress;
	idle_union->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle; // guardem l'adreça de cpu_idle per fer el task switch
    idle_union->stack[KERNEL_STACK_SIZE - 2] = 0; // guardem valor inicial registre EBP (0)
    idle_pcb->kernel_esp = (unsigned long) &(idle_union->stack[KERNEL_STACK_SIZE - 2]); // guardem l'adreça de l'EBP fals que hem posat a la pila
    idle_pcb->quantum = DEFAULT_QUANTUM;
    idle_pcb->state = ST_READY;

    idle_task = idle_pcb;
}

void init_task1(void)
{
    int Dir = alloc_frame(); // reservem un frame per el nou dir
    page_table_entry *DirAddress = (page_table_entry *) (Dir << 12); // definim un punter al dir d'init
    clear_page_table(DirAddress);

    int SystemPT = alloc_frame(); // reservem un frame per la taula de pagines de sistema
    page_table_entry *SPTAddress = (page_table_entry *) (SystemPT << 12); // definim un punter a la taula de pagines de sistema
    clear_page_table(SPTAddress);

    int UserPT = alloc_frame(); // reservem un frame per la taula de pagines d usuari
    page_table_entry *UPTAddress = (page_table_entry *) (UserPT << 12); // definim un punter a la taula de pagines d'usuari
    clear_page_table(UPTAddress);

    set_kernel_pages(SPTAddress); // omplim la taula de pagines de sistema amb les pagines del kernel
    set_user_pages(UPTAddress); // reservem frames pel codi i dades d'usuari i els assignem a la taula de pagines d'usuari

    set_ss_pag(DirAddress, 0, SystemPT, 0); // nova entrada a dirInit que asigna entrada 0 del dir amb la taula de pagines de sistema
    set_ss_pag(DirAddress, 1, UserPT, 1); // nova entrada a dirInit que asigna entrada 1 del dir amb la taula de pagines d'usuari

    int PCB = alloc_frame(); // reservem espai pel pcb
    union task_union *task1_union = (union task_union *) (PCB << 12); // associem a aquest espai el union
    struct task_struct *task1 = &(task1_union->task);

    set_ss_pag(SPTAddress, Dir, Dir, 0); // nova entrada a la taula de pagines de sistema amb dirInit -> dirInit
    set_ss_pag(SPTAddress, SystemPT, SystemPT, 0); // creem una entrada a la taula de pagines de sistema per ella mateixa -> ella mateixa
    set_ss_pag(SPTAddress, UserPT, UserPT, 0); // creem una entrada a la taula de pagines de sistema per la taula d'usuari -> taula d'usuari
    set_ss_pag(SPTAddress, PCB, PCB, 0); // creem una entrada a la taula de pagines de sistema per pcb -> pcb

    task1->PID = 1;
    task1->dir_pages_baseAddr = DirAddress;
    task1->quantum = DEFAULT_QUANTUM;
    task1->state = ST_RUN;
    ticks_restants = task1->quantum;

    set_cr3(DirAddress); // el dir d'init passa a ser l'espai d'adreces actiu

    init_task = task1;

    tss.esp0 = (unsigned long) &(task1_union->stack[KERNEL_STACK_SIZE]); // guardem el top de la pila de sistema d'init per les entrades des d'usuari
}

void inner_task_switch(union task_union* new)
{
	page_table_entry * newDir = get_DIR(&(new->task));

	tss.esp0 = (unsigned long) &(new->stack[KERNEL_STACK_SIZE]); //apunta al final del stac del nou proces
	set_cr3(newDir); // canvi al nou directori

	/**current()->kernel_esp = %ebp
	 * %esp = new->task.kernel_esp
	 */
	switch_stack(&current()->kernel_esp, new->task.kernel_esp);
}

void init_sched()
{
	INIT_LIST_HEAD(&ready_queue);
}

int assign_PID(void)
{
    return next_pid++;
}

/* retorna el task_struct que conte el node 'l' */
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
	return list_entry(l, struct task_struct, list); // restem a 'l' l'offset del camp list dins del task_struct
}

void update_process_state_rr(struct task_struct *t, struct list_head *dst_queue)
{
    if (t->state != ST_RUN) list_del(&t->list);
    
    if (dst_queue != NULL) 
    {
        list_add_tail(&t->list, dst_queue);
        t->state = ST_READY;
    } 
    else
    {
        t->state = ST_RUN;
    }
}

void update_sched_data_rr(void) 
{
    if (ticks_restants > 0) --ticks_restants; // no baixa mai de 0
}

/*decideix si fa falta cambiar de proces*/
int needs_sched_rr(void)
{
    // quantum acabat o som idle, i hi ha algu esperant a la ready_queue
    if ((ticks_restants == 0 || (current()->PID == 0)) && !list_empty(&ready_queue)) return 1;
    else return 0;
}

/*selecciona el seguent proces i cambia a ell*/
void sched_next_rr(void)
{
    struct task_struct *new;
    if (list_empty(&ready_queue))
    {
        new = idle_task; // idle no esta a cap cua, no fem update_process_state_rr
    }
    else
    {
        new = list_head_to_task_struct(list_first(&ready_queue));
        update_process_state_rr(new, NULL);
    }
    
    ticks_restants = get_quantum(new); // abans del task_switch, el fill i idle la primera vegada no tornen aqui
    task_switch((union task_union *)new);
}

void schedule(void)
{
    update_sched_data_rr();

    if (needs_sched_rr()) 
    {
        if (current()->PID != 0) update_process_state_rr(current(), &ready_queue); // idle no va mai a la ready_queue
        sched_next_rr();
    }
}

int get_quantum(struct task_struct *t)
{
    return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum)
{
    t->quantum = new_quantum;
}
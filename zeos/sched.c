/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>

char initial_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned(4096))); // Space for the initial system stack

struct list_head ready_queue;
struct task_struct *init_task;
struct task_struct *idle_task;

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
	int Dir = alloc_frame();
	page_table_entry *DirAddress = (page_table_entry *) (Dir << 12);
	page_table_entry *SystemPT = get_PT(init_task);
	set_ss_pag(SystemPT, Dir, Dir, 0);
	clear_page_table(DirAddress);

	int PCB = alloc_frame();
	set_ss_pag(SystemPT, PCB, PCB, 0);
	union task_union *idle_union = (union task_union *) (PCB << 12);
	struct task_struct *idle_pcb = &(idle_union->task);

	idle_pcb->PID = 0;
    idle_pcb->dir_pages_baseAddr = DirAddress;
	idle_union->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle; // guardem l'adreça de cpu_idle per fer el task switch
    idle_union->stack[KERNEL_STACK_SIZE - 2] = 0; // guardem valor inicial registre EBP (0)
    idle_pcb->kernel_esp0 = (unsigned long) &(idle_union->stack[KERNEL_STACK_SIZE - 2]); // guardem la posició del registre ESP al camp kernel_esp del PCB

    idle_task = idle_pcb;
}

void init_task1(void)
{
	int Dir = alloc_frame();
	page_table_entry *DirAddress = (page_table_entry *) (Dir << 12);
	clear_page_table(DirAddress);

	int SystemPT = alloc_frame();
	page_table_entry *SPTAddress = (page_table_entry *) (SystemPT << 12);
	clear_page_table(SPTAddress);

	int UserPT = alloc_frame();
	page_table_entry *UPTAddress = (page_table_entry *) (UserPT << 12);
	clear_page_table(UPTAddress);

	set_kernel_pages(SPTAddress);
	set_user_pages(UPTAddress);

	set_ss_pag(DirAddress, SystemPT, SystemPT, 0);
	set_ss_pag(DirAddress, UserPT, UserPT, 1);

	int PCB = alloc_frame();
	union task_union *task1_union = (union task_union *) (PCB << 12);
	struct task_struct *task1 = &(task1_union->task);

	set_ss_pag(SPTAddress, Dir, Dir, 0);
	set_ss_pag(SPTAddress, SystemPT, SystemPT, 0);
    set_ss_pag(SPTAddress, UserPT, UserPT, 0);
    set_ss_pag(SPTAddress, PCB, PCB, 0);

	task1->PID = 1;
	task1->dir_pages_baseAddr = DirAddress;

	set_cr3(DirAddress);

	init_task = task1;

	tss.esp0 = &(task1_union->stack[KERNEL_STACK_SIZE]);
}

void inner_task_switch(union task_union* new)
{
	page_table_entry * newDir = get_DIR(&(new->task));

	tss.esp0 = (unsigned long) &(new->stack[KERNEL_STACK_SIZE]);

	set_cr3(newDir);

	/**current()->kernel_esp = %ebp
	 * %esp = new->task.kernel_esp
	 */
	switch_stack();
}

void init_sched()
{
	
}
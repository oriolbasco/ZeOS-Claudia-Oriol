/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <hardware.h>

char initial_stack[KERNEL_STACK_SIZE] __attribute__ ((aligned(4096))); // Space for the initial system stack

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
	//page directory de idle
	int Dir = alloc_frame();
	page_table_entry *DirAddress = (page_table_entry *) (Dir << 12);
	clear_page_table(DirAddress);

	//en la entrada 0 del directorio de idle debe acabar apareciendo el mismo frame de System PT que usa init
	DirAddress[0].entry = 0;
	DirAddress[0].bits.pbase_addr = init_task->SPTAddress;
	DirAddress[0].bits.present = 1;
	DirAddress[0].bits.rw = 1;
	DirAddress[0].bits.user = 0;

	int PCB = alloc_frame();
	union task_union *idle_union = (union task_union *) (PCB << 12);
	struct task_struct *idle_task = &(idle_union->task);

	set_ss_pag(SPTAddress, Dir, Dir, 0);
	set_ss_pag(SPTAddress, PCB, PCB, 0);

	idle_task->PID = 0;
	idle_task->dir_pages_baseAddr = DirAddress;

	//preparar su stack de sistema?
	idle_union->stack[KERNEL_STACK_SIZE-1] = &cpu_idle;
	idle_union->stack[KERNEL_STACK_SIZE-2] = 0; //valor del reg ebp
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

	//omplim taules de pagines(memoria kernell i user)
	set_kernel_pages(SPTAddress);
	set_user_pages(UPTAddress);

	//conectem les dues al page directory
    DirAddress[0].entry = 0;
    DirAddress[0].bits.pbase_addr = SystemPT;
    DirAddress[0].bits.present = 1;
    DirAddress[0].bits.rw = 1;
    DirAddress[0].bits.user = 0;

    DirAddress[1].entry = 0;
    DirAddress[1].bits.pbase_addr = UserPT;
    DirAddress[1].bits.present = 1;
    DirAddress[1].bits.rw = 1;
    DirAddress[1].bits.user = 1;

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

<<<<<<< HEAD
=======
void inner_task_switch(union task_union* new)
{
	page_table_entry * newDir = get_DIR(&(new->task));

	//apunta al final del stac del nou proces
	tss.esp0 = (unsigned long) &(new->stack[KERNEL_STACK_SIZE]);
	//cambia el page directoy al nuevo
	set_cr3(newDir);

	/**current()->kernel_esp0 = %ebp
	 * %esp = new->task.kernel_esp0 //canvi de stack al nou proces (per on anava)
	 */
	switch_stack();
}
>>>>>>> 4e9c400 (getpid fet)

void init_sched()
{

}

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


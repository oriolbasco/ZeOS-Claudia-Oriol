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
	set_user_pages(DirAddress);
	set_ss_pag(SPTAddress, Dir, Dir, 0);

	DirAddress[0].entry = SPTAddress;
	DirAddress[1].entry = UPTAddress;

	int PCB = alloc_frame();

	struct task_union *task1_union = (struct task_union *) (PCB << 12);
	struct task_struct *task1 = &(task1_union->task);
	task1->PID = 1;

	TSS.esp0 = &(task1_union->stack[1024]);
}


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


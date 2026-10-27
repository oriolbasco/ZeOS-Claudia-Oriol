/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>
#include <errno.h>

/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %edx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %ebx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
	pushl %gs; \
	pushl %fs; \
	pushl %es; \
	pushl %ds; \
	pushl %eax; \
	pushl %ebp; \
	pushl %edi; \
	pushl %esi; \
	pushl %ebx; \
	pushl %ecx; \
	pushl %edx; \
	movl $__KERNEL_DS, %edx;    \
	movl %edx, %ds;           \
	movl %edx, %es

#define RESTORE_ALL \
	popl	%edx; \
	popl	%ecx; \
	popl	%ebx; \
	popl	%esi; \
	popl	%edi; \
	popl	%ebp; \
	popl	%eax; \
	popl	%ds; \
	popl	%es; \
	popl	%fs; \
	popl	%gs;  
	
#define EOI \
	movb $0x20, %al; \
	outb %al, $0x20;
	
ENTRY(keyboard_handler)
	SAVE_ALL
	EOI

	call keyboard_routine
	
	RESTORE_ALL
	iret

ENTRY(clock_handler)
	SAVE_ALL
	EOI

	call clock_routine
	
	RESTORE_ALL
	iret

ENTRY(ret_from_fork)
    RESTORE_ALL
    iret
	
ENTRY(my_page_fault_handler)
	SAVE_ALL
	
	pushl %esp // dir de top pila
	call my_page_fault_routine
	
	addl $4, %esp // esborrem el parametre de la funcio

	RESTORE_ALL
	
	addl $4, %esp // hem de treure el error code de la pila
	iret

ENTRY(task_switch)
	pushl	%ebp
	movl	%esp, %ebp

	pushl	%esi // guardem ESI, EDI i EBX
	pushl	%edi
	pushl	%ebx

	movl	8(%ebp), %eax // parametre union task_union *new de la funcio inner_task_switch
	pushl	%eax

	call	inner_task_switch

	addl	$4, %esp // esborrem el parametre de la funcio inner_task_switch

	popl	%ebx
	popl	%edi
	popl 	%esi
	popl	%ebp

	ret

ENTRY(switch_stack)
	pushl	%ebp
	movl	%esp, %ebp

	movl	8(%ebp), %eax // agafem parametre &current()->kernel_esp
    movl	%ebp, (%eax) // curent()->kernel_esp = ebp

	movl	12(%ebp), %esp // esp = new()->task.kernel_esp

	popl	%ebp
	ret
	

//el handler de todas las system calls
ENTRY(system_call_handler)
	SAVE_ALL
	cmpl $0, %EAX
	jl err
	cmpl $MAX_SYSCALL, %EAX
	jg err
	call *sys_call_table(, %EAX, 0x04)
	jmp fin
err:
	movl $-ENOSYS, %EAX
fin:
	movl %EAX, 0x18(%esp)
	RESTORE_ALL
	iret
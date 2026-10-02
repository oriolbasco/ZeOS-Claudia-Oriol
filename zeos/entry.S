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
	
ENTRY(my_page_fault_handler)
	SAVE_ALL
	
	pushl %esp // dir de top pila
	call my_page_fault_routine
	
	addl $4, %esp // esborrem el parametre de la funcio

	RESTORE_ALL
	
	addl $4, %esp // hem de treure el error code de la pila
	iret

ENTRY(system_call_handler)
	SAVE_ALL

	cmpl $0, %eax // mirar si el index de la sys call es negatiu
	jl	err
	cmpl $MAX_SYSCALL, %eax // mirar si el index de la sys call es mes gran que el maxim
	jg	err

	pushl %ebx
	call *sys_call_table(, %eax, 4) // entrem a la syscall pel seu index
	addl $4, %esp
	jmp fin

err: 
	movl $-ENOSYS, %eax
fin:
	movl %eax, 0x18(%esp) // guardem directament a la pila el valor de eax pel return ja que sino amb el restore all s esborraria
	RESTORE_ALL
	iret
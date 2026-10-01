/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>
#include <libc.h>

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

ENTRY(write)
	pushl %ebp
	movl %esp, %ebp

	pushl %ebx // hem de guardar el valor actual d ebx pq necessitem el registre pel pas de parametres

	lea 8(%ebp), %ebx // posem a ebx l adreça de la pila on començen els parametres

	movl $4, %eax // pos a la sys_call_table
	int	$0x93 // activem la trap

	popl %ebx

	cmp $0, %eax // mirem si el return value del handler es negatiu (error)
	jge	ok

	negl %eax // agafem el valor absolut
	movl %eax, errno // el posem a variable errno
	movl $-1, %eax // retornem -1 per indicar que hi ha error

ok:
	popl %ebp
	ret

ENTRY(write_handler)
	SAVE_ALL

	cmpl $0, %eax // mirar si el index de la sys call es negatiu
	jl	err
	cmpl $MAX_SYSCALL, %eax // mirar si el index de la sys call es mes gran que el maxim
	jg	err

	// posem el punter als parametres (pila d usuari)
	pushl %ebx

	call *sys_call_table(, %eax, 0x04) // entrem a la syscall pel seu index
	addl $4, %esp // treure parametre de la pila
	jmp fin

err: 
	movl $-ENOSYS, %eax
fin:
	movl %eax, 0x18(%esp) // guardem directament a la pila el valor de eax pel return ja que sino amb el restore all s esborraria
	RESTORE_ALL
	iret
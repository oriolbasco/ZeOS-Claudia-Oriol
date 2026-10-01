# 0 "entry.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 1 "include/libc.h" 1
# 9 "include/libc.h"
int errno = 0;

void itoa(int a, char *b);

int strlen(char *a);
# 8 "entry.S" 2
# 71 "entry.S"
.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20;

 call keyboard_routine

 popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret

.globl clock_handler; .type clock_handler, @function; .align 0; clock_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20;

 call clock_routine

 popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret

.globl my_page_fault_handler; .type my_page_fault_handler, @function; .align 0; my_page_fault_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es

 pushl %esp
 call my_page_fault_routine

 addl $4, %esp

 popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;

 addl $4, %esp
 iret

.globl write; .type write, @function; .align 0; write:
 pushl %ebp
 movl %esp, %ebp

 pushl %ebx

 lea 8(%ebp), %ebx

 movl $4, %eax
 int $0x93

 popl %ebx

 cmp $0, %eax
 jge ok

 negl %eax
 movl %eax, errno
 movl $-1, %eax

ok:
 popl %ebp
 ret

.globl write_handler; .type write_handler, @function; .align 0; write_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es

 cmpl $0, %eax
 jl err
 cmpl $MAX_SYSCALL, %eax
 jg err


 pushl %ebx

 call *sys_call_table(, %eax, 0x04)
 addl $4, %esp
 jmp fin

err:
 movl $-ENOSYS, %eax
fin:
 movl %eax, 0x18(%esp)
 popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret

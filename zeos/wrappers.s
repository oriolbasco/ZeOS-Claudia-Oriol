# 0 "wrappers.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2
# 1 "include/segment.h" 1
# 3 "wrappers.S" 2

.globl write; .type write, @function; .align 0; write:

  pushl %ebp
  mov %esp, %ebp

  leal 8(%ebp), %ebx

  mov $4, %eax
  INT $0x93


  cmp 0, %eax
  jge continuar

  negl %eax
  mov %eax, errno
  mov $-1, %eax

continuar:
  mov %ebp, %esp
  popl %ebp
  ret

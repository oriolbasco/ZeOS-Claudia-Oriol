#include <asm.h>
#include <segment.h>

ENTRY(write)
  //registers EDX, ECX, EBX, ESI, EDI, EBP.
  pushl %ebp
  mov %esp, %ebp
  
  leal 8(%ebp), %ebx //puntero a param de pila user.
  
  mov $4, %eax //funcionalidad;
  INT $0x93 //entrada SO
  
  //si hay un error en la llamada!!
  cmp 0, %eax
  jge continuar
//if:
  negl %eax
  mov %eax, errno
  mov $-1, %eax

continuar:
  mov %ebp, %esp
  popl %ebp
  ret

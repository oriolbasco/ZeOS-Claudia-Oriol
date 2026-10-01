#include <asm.h>
#include <segment.h>

ENTRY(write)
  //guardamos variables para tenerlas en modo sistema, nose q quiere decir esto!!!
  //registers EDX, ECX, EBX, ESI, EDI, EBP.
  mov 8(%ebp), %edx
  mov 12(%ebp), %ecx
  mov 16(%ebp), %ebx
  
  mov $4, %eax //funcionalidad;
  INT $0x80 //entrada SO
  
  //si hay un error en la llamada!!
  cmp 0, %eax
  jge continuar
//if:
  mov -%eax, errno
  mov $-1, %eax

//esta bien esta etiquetaa??
continuar:
  ret

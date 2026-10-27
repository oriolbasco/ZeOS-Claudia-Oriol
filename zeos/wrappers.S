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
	pushl %ebx // hem de guardar el valor actual d ebx pq necessitem el registre pel pas de parametres

	lea 8(%ebp), %ebx // posem a ebx l adreça de la pila on començen els parametres

	movl $4, %eax // pos a la sys_call_table
	int	$0x93 // activem la trap

	popl %ebx

	cmp $0, %eax // mirem si el return value del handler es negatiu (error)
	jge	ok

	negl %eax // agafem el valor absolut
    movl %eax, errno
	movl $-1, %eax // retornem -1 per indicar que hi ha error

ok:
	popl %ebp
	ret


ENTRY(gettime)
	pushl %ebp
	movl %esp, %ebp

	movl $10, %eax // pos a la sys_call_table 
	int	$0x93 // activem la trap
	
	popl %ebp
	ret

ENTRY(getpid)
	pushl %ebp
	movl %esp, %ebp

	movl $20, %eax // pos a la sys_call_table 
	int	$0x93 // activem la trap

	popl %ebp
	ret


ENTRY(fork)
	pushl %ebp
	movl %esp, %ebp

	movl $2, %eax // pos a la sys_call_table 
	int	$0x93 // activem la trap

	popl %ebp
	ret

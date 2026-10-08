#include <asm.h>

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

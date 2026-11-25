#include <asm.h>
#include <segment.h>

ENTRY(write)
	pushl	%ebp
	movl	%esp, %ebp

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

	// Convertir l error del kernel a -1 y errno. 
	cmpl $0, %eax
	jge fork_ok

	negl %eax
	movl %eax, errno
	movl $-1, %eax

fork_ok:
	popl %ebp
	ret

ENTRY(exit)
	pushl %ebp
	movl %esp, %ebp

	movl $1, %eax // pos a la sys_call_table 
	int	$0x93 // activem la trap

	popl %ebp
	ret


ENTRY(read)
	pushl %ebp
	movl %esp, %ebp

	pushl %ebx

	// posem a ebx l adreça de la pila on començen els parametres b i maxchars
	lea 8(%ebp), %ebx

	movl $3, %eax
	int $0x93

	popl %ebx

	// Convertir l error del kernel a -1 y errno. 
	cmpl $0, %eax
	jge read_ok

	negl %eax
	movl %eax, errno
	movl $-1, %eax

read_ok:
	popl %ebp
	ret


ENTRY(malloc)
	pushl %ebp
	movl %esp, %ebp

	pushl %ebx

	lea 8(%ebp), %ebx

	movl $5, %eax
	int $0x93

	popl %ebx

	// Convertir l error del kernel a -1 y errno. 
	cmpl $0, %eax
	jge read_ok

	negl %eax
	movl %eax, errno
	movl $-1, %eax

read_ok:
	popl %ebp
	ret


ENTRY(free)
	pushl %ebp
	movl %esp, %ebp

	pushl %ebx

	lea 8(%ebp), %ebx

	movl $6, %eax
	int $0x93

	popl %ebx

	// Convertir l error del kernel a -1 y errno. 
	cmpl $0, %eax
	jge read_ok

	negl %eax
	movl %eax, errno
	movl $-1, %eax

read_ok:
	popl %ebp
	ret
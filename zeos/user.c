#include <libc.h>

char buff[24];

int pid;
struct sys_stack{
  unsigned int edx;
  unsigned int ecx;
  unsigned int ebx;
  unsigned int esi;
  unsigned int edi;
  unsigned int ebp;
  unsigned int eax;
  unsigned int ds;
  unsigned int es;
  unsigned int fs;
  unsigned int gs;
  
  unsigned int error_code;
  unsigned int eip;
  unsigned int cs;
  unsigned int eflags;
  unsigned int oldesp;
  unsigned int oldss;
};

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

    
  while(1) { }
}

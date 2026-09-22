#include <libc.h>

char buff[24];

int pid;
struct sys_stack{
  register edx;
  register ecx;
  register ebx;
  register esi;
  register edi;
  register ebp;
  register eax;
  register ds;
  register es;
  register fs;
  register gs;
  register eip;
  register cs;
  register eflags;
  register oldesp;
  register oldss;
};

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

    
  while(1) { }
}

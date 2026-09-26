/*
 * system.h - Capçalera del mòdul principal del sistema operatiu
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <types.h>
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

extern TSS         tss;
extern Descriptor* gdt;

#endif  /* __SYSTEM_H__ */

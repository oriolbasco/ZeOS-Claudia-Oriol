/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

int errno = 0;

void itoa(int a, char *b);

int strlen(char *a);

#endif  /* __LIBC_H__ */

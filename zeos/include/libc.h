/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

extern int errno;

void itoa(int a, char *b);
int write(int fd, char *buffer, int size);
int gettime();
int strlen(char *a);

void perror(void);

int write (int fd, char * buffer, int size);

#endif  /* __LIBC_H__ */

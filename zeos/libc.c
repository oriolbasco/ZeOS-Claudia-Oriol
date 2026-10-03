/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>
#include <errno.h>
#include <io.h>

int errno = 0;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror(void)
{
  switch (errno)
  {
  case EBADF:
    write(1, "fd incorrecte\n", 14);
    break;
  case EINVAL:
    write(1, "buffer o size incorrectes\n", 26);
    break;
  case ENOSYS:
    write(1, "funcio no implementada\n", 24);
    break;
  case 0:
    write(1, "la sys call s'ha executat correctament\n", 39);
    break;
  default:
    write(1, "error desconegut\n", 18);
    break;
  }
}
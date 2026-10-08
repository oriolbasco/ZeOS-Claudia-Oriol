#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

//proba de write
  if (write(1, "\nProva write\n", 13) < 0) perror();
  if (write(5, "\nProva write erronia\n", 13) < 0) perror(); // error fd incorrecte
  
  //proba de gettime
  if (write(1, "\nProva gettime: \n", sizeof("\nProva gettime: \n")) < 0) perror();
  
  int t1 = gettime();
  
  itoa(t1, buff);
  write(1, buff, sizeof(buff));
  write(1, "\n", 1);

  for(int i = 0; i < 500000; ++i); //delay;
  
  int t2 = gettime();
  
  itoa(t2, buff);
  write(1, buff, sizeof(buff));
  write(1, "\n", 1);

  // joc de prova page fault
  char* p = 0;
  *p = 'x';

  while(1) { }
}

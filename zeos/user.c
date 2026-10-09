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
  
  char buffTestSize[500]; // prova buffer mes que 256
  for (int i = 0; i < 500; ++i) buffTestSize[i] = 'a';
  if (write(1,buffTestSize,500) < 0) perror();

  if (write(1, nullptr, 10) < 0) perror(); // prova punter buffer incorrecte
  if (write(1, "Test", -5) < 0) perror(); // prova size dolent

  //proba de gettime
  if (write(1, "\nProva gettime: \n", sizeof("\nProva gettime: \n")) < 0) perror();
  
  int t1 = gettime();
  
  itoa(t1, buff);
  write(1, buff, strlen(buff));
  write(1, "\n", 1);

  for(int i = 0; i < 500000; ++i); //delay;
  
  int t2 = gettime();
  
  itoa(t2, buff);
  write(1, buff, strlen(buff));
  write(1, "\n", 1);

  // joc de prova page fault
  char* p = 0;
  *p = 'x';

  while(1) { }
}

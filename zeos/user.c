#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  //char* p = 0;
  //*p = 'x';
  //int ret = write(1, buff, sizeof(buff));
  /*
  char buffTestSize[500]; // prova buffer mes que 256
  for (int i = 0; i < 500; ++i) buffTestSize[i] = 'a';
  //if (write(1,buffTestSize,500) < 0) perror();

  if (write(1, 0, 10) < 0) perror(); // prova punter buffer incorrecte
  if (write(1, "Test", -5) < 0) perror(); // prova size dolent

  //proba de gettime
  if (write(1, "\nProva gettime: \n", sizeof("\nProva gettime: \n")) < 0) perror();
  
  int t1 = gettime();
  
  itoa(t1, buff);
  if(write(1, buff, strlen(buff)) < 0) perror();
  write(1, "\n", 1);

  for(int i = 0; i < 500000; ++i); //delay;
  
  int t2 = gettime();
  
  itoa(t2, buff);
  write(1, buff, strlen(buff));
  write(1, "\n", 1);

  // joc de prova page fault
  //char* p = 0;
  //*p = 'x';
*/
  // joc de prova getpid + fork
  if (write(1, "\n[init] getpid = ", strlen("\n[init] getpid = ")) < 0) perror();
  itoa(getpid(), buff); // ha de ser 1
  if (write(1, buff, strlen(buff)) < 0) perror();

  int ret = fork();
  if (ret < 0) perror();
  else if (ret == 0)
  {
    if (write(1, "\n[fill] fork ha retornat 0, getpid = ", strlen("\n[fill] fork ha retornat 0, getpid = ")) < 0) perror();
    itoa(getpid(), buff); // ha de ser 2
    if (write(1, buff, strlen(buff)) < 0) perror();
  }
  else
  {
    if (write(1, "\n[pare] fork ha retornat ", strlen("\n[pare] fork ha retornat ")) < 0) perror();
    itoa(ret, buff); // PID del fill, ha de ser 2
    if (write(1, buff, strlen(buff)) < 0) perror();

    if (write(1, ", getpid = ", strlen(", getpid = ")) < 0) perror();
    itoa(getpid(), buff); // ha de ser 1
    if (write(1, buff, strlen(buff)) < 0) perror();
  }

  // joc de prova planificador round robin: pare i fill escriuen el seu PID alternant-se
  if (write(1, "\n", strlen("\n")) < 0) perror();
  while(1)
  {
    for (int i = 0; i < 500000; ++i); // espera

    itoa(getpid(), buff);
    if (write(1, buff, strlen(buff)) < 0) perror();
    if (write(1, " ", strlen(" ")) < 0) perror();
  }
}

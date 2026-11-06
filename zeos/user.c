#include <libc.h>

char buff[24];

int pid;

int globalVar = 20;
int __attribute__ ((__section__(".text.main")))
main(void)
{
/*
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  //========================================prova accedir a adreça 0=====================================================
  /*
  char* p = 0;
  *p = 'x';
  int ret = write(1, buff, sizeof(buff));
  */
  //==================================================prova WRITE buffer mes que 256===================================================
  /*
  char buffTestSize[500];
  for (int i = 0; i < 500; ++i) buffTestSize[i] = 'a';
  if (write(1,buffTestSize,500) < 0) perror();

  if (write(1, 0, 10) < 0) perror(); // prova punter buffer incorrecte
  if (write(1, "Test", -5) < 0) perror(); // prova size dolent

  //==========================================proba de gettime=====================================================================
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

*/

/*
  // ================================================joc de prova getpid + fork====================================================
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

  // ==============================joc de prova planificador + exit: el fill fa exit despres de 10 escriptures====================================
  if (write(1, "\n", strlen("\n")) < 0) perror();
  for (int n = 0; n < 30; ++n)
  {
    for (int i = 0; i < 500000; ++i); // espera

    itoa(getpid(), buff);
    if (write(1, buff, strlen(buff)) < 0) perror();
    if (write(1, " ", strlen(" ")) < 0) perror();

    if (ret == 0 && n == 9)
    {
      if (write(1, "\n[fill] exit\n", strlen("\n[fill] exit\n")) < 0) perror();
      exit();
    }
  }

  //=========================================joc de prova idle: no queda cap proces==================================================0
  exit(); //proces pare fa exit
  if (write(1, "\n[pare] exit, a partir d'ara idle\n", strlen("\n[pare] exit, a partir d'ara idle\n")) < 0) perror();



  //======================================joc de proba modificar variable local i global(fill)==========================================
  int localVar = 10;

  int pid = fork();
  if(pid < 0) perror();

  if(pid == 0) {

    //abans:
    if (write(1, "\n[fill] variable global i local: ", strlen("\n[fill] variable global i local: ")) < 0) perror();
    itoa(localVar, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();

    itoa(globalVar, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();

    globalVar = 200;
    localVar = 100;

    //despres:


    if (write(1, "\n[fill] variable global i local: ", strlen("\n[fill] variable global i local: ")) < 0) perror();
    itoa(localVar, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();

    itoa(globalVar, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();
  }
  
  
  else {
    if (write(1, "\n[pare] variable global i local: ", strlen("\n[pare] variable global i local: ")) < 0) perror();
    itoa(localVar, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();

    itoa(globalVar, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();
  }


//===========================================CREAR FILLS INFINITS=====================================================
  int pid = fork();
  while(pid > 0) {
    pid = fork();

    if (write(1, "\npid fill:\n", strlen("\npid fill:\n")) < 0) perror();
    itoa(pid, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();
  }
*/
  //====================================================unic proces executantse(no volem idle)========================================
  while(1) {
    int pid = getpid();
    if (write(1, "\npid actual:\n", strlen("\npid actual:\n")) < 0) perror();
    itoa(pid, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();
  }
}
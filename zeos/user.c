#include <libc.h>
#include <keys.h>

char buff[24];

int pid;

int globalVar = 20;

void print(char *s)
{
  if (write(1, s, strlen(s)) < 0) perror();
}

// mostra els caracters llegits
void print_chars(char *b, int n)
{
  print("[");
  for (int i = 0; i < n; i++)
  {
    switch (b[i])
    {
      case KEY_UP: print("<UP>"); break;
      case KEY_DOWN: print("<DOWN>"); break;
      case KEY_LEFT: print("<LEFT>"); break;
      case KEY_RIGHT: print("<RIGHT>"); break;
      default: if (write(1, &b[i], 1) < 0) perror(); break;
    }
  }
  print("]");
}

// bucle d'espera per teclejar abans del read
void espera(int vegades)
{
  for (int v = 0; v < vegades; ++v)
    for (int i = 0; i < 500000; ++i);
}

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

/*
  // joc de prova quants fills podem crear: els fills es queden vius i el pare fa fork fins que falla
  int fills = 0;
  pid = fork();
  while (pid > 0)
  {
    ++fills;
    pid = fork();
  }
*/

//=====================================================JOC DE PROVES READ=====================================================
//================================proba read un proces=======================================================
/*
char b[5];
char numero[24];

write(1, "\nABANS DEL READ\n", strlen("\nABANS DEL READ\n"));

int n = read(b, 5);

write(1, "\nRetorn de read: ", strlen("\nRetorn de read: "));
itoa(n, numero);
write(1, numero, strlen(numero));
write(1, "\n", 1);

if (n < 0) perror();

else if (n <= sizeof(b)) {
  write(1, "Contingut: [", strlen("Contingut: ["));
  write(1, b, n);
  write(1, "]\n", 2);
}
*/

//============================proba read dos procesos=======================================
/*
char b[5];
int pid = fork();

if (pid < 0) perror();

if (pid == 0) print("\nFILL: demana 5 caracters\n");
else print("\nPARE: demana 5 caracters\n");

int n = read(b, 5); //els dos llegeixen

if (n < 0) perror();
else 
{
  char message[32];
  char *prefix;

  if (pid == 0) prefix = "\nFILL ha llegit: [";

  else prefix = "\nPARE ha llegit: [";

  int len = 0;

  while (prefix[len] != '\0') 
  {
    message[len] = prefix[len];
    len++;
  }

  for (int i = 0; i < n; i++) message[len++] = b[i];

  message[len++] = ']';
  message[len++] = '\n';

  if (write(1, message, len) < 0) perror();
}

//fill exit
if(pid == 0) 
{
  print("\nFILL acaba");
  exit();
}
*/
//==========================proba read mes gran que el buffer (CBUF_SIZE 10)===================
/*
char big[15];
print("\nREAD de 15 (buffer de 10): escriu 15 tecles\n");
int nb = read(big, 15);
if (nb < 0) perror();
else {
  print("\nHa llegit: [");
  write(1, big, nb);
  print("]\n");
}
  */
//===============================================tecles ja al buffer abans del read==========================
/*
  char b[16];
  print("\ntecleja 3 tecles ARA (durant l'espera)...");
  espera(800);
  print("\nfem read(b, 3): ");
  int n = read(b, 3);
  print("retorna ");
  itoa(n, buff);
  print(buff);
  print(" -> "); 
  print_chars(b, n);
*/

//==========================================proba fletxes========================================
// adalt, abaix, esquerra, dreta -> UP, DOWN, LEFT, RIGHT.
/*
  char b[16];
  print("\nPrem adalt, abaix, esquerra, dreta: ");
  int n = read(b, 4);
  print("\nread ha retornat "); 
  itoa(n, buff);
  print(buff);
  print(" -> "); print_chars(b, n);
*/

//==========================================proba malloc========================================
print("proba malloc(5000)...\n");
char *p = malloc(5000);

if (p == 0) print("ERROR: malloc(5000) devolvio NULL\n");
else 
{
  p[4095] = 'A';
  p[4096] = 'B';
  p[4999] = 'C';

  if (p[4095] == 'A' && p[4096] == 'B' && p[4999] == 'C') print("OK: valors llegits be\n");
  else print("ERROR: valores no OK\n");
}

if (malloc(5 * 1024 * 1024) == 0) print("OK: reserva masa gran\n");
else print("ERROR: s accepta una reserva masa gran\n");

//====================================================unic proces executantse(no volem idle)========================================
  while(1) {
    /*int pid = getpid();
    if (write(1, "\npid actual:\n", strlen("\npid actual:\n")) < 0) perror();
    itoa(pid, buff);
    if (write(1, buff, strlen(buff)) < 0) perror();
    */
  }
}
#include <io.h>
#include <utils.h>
#include <list.h>

// Queue for blocked processes in I/O 
struct list_head blocked;

/* ---- Parser de sequencies ANSI (subconjunt ECMA-48, man 4 console_codes) ----
 * Suportat:  ESC [ fila ; col H   posiciona el cursor (1-based)
 *            ESC [ n ; n ... m    colors: 0 reset, 30-37 primer pla, 40-47 fons
 *            '\b'                 esborra el caracter anterior (backspace)
 * L'estat es static: una sequencia pot arribar partida entre dues crides
 * (o entre dos chunks del mateix sys_write). */

#define ESC      0x1B
#define MAX_PARAMS 4

enum ansi_state { ST_NORMAL, ST_ESC, ST_PARAMS };

static enum ansi_state state = ST_NORMAL;
static int params[MAX_PARAMS];
static int nparams;

/* ANSI i VGA tenen els bits 0 i 2 intercanviats */
static const Byte ansi_to_vga[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
static Byte fg = 2, bg = 0; /* colors VGA actuals, igual que attr inicial de io.c */

/* S'executa quan arriba la lletra final; params[0..nparams-1] ja estan plens.
 * Parametre absent o 0 = valor per defecte (com a la consola de Linux). */
static void esc_execute(char final)
{
  int i;
  switch (final)
  {
    case 'H':
    {
      int row = (nparams > 0 && params[0] > 0) ? params[0] : 1;
      int col = (nparams > 1 && params[1] > 0) ? params[1] : 1;
      set_cursor(col - 1, row - 1);
      break;
    }
    case 'm':
      if (nparams == 0) { fg = 2; bg = 0; }
      for (i = 0; i < nparams; i++)
      {
        int p = params[i];
        if (p == 0) { fg = 2; bg = 0; }
        else if (p >= 30 && p <= 37) fg = ansi_to_vga[p - 30];
        else if (p >= 40 && p <= 47) bg = ansi_to_vga[p - 40];
      }
      set_color(fg, bg);
      break;
    default: break; /* comanda desconeguda: s'ignora */
  }
}

/* Processa UN caracter que l'usuari ha escrit a la consola. */
static void ansi_feed(char c)
{
  switch (state)
  {
    case ST_NORMAL:
    {
      if (c == ESC) state = ST_ESC;
      else if (c == '\b') delete_char();
      else printc(c);
      break;
    }
    case ST_ESC:
    {
      if (c == '[') 
      {
        state = ST_PARAMS;
        nparams = 0;
        for (int i = 0; i < MAX_PARAMS; ++i) params[i] = 0;
      }
      else 
      {
        state = ST_NORMAL;
      }
      break;
    }
    case ST_PARAMS:
    {
      if (c >= '0' && c <= '9')
      {
        params[nparams] = params[nparams] * 10 + (c - '0');
      }
      else if (c == ';')
      {
        if (nparams < MAX_PARAMS - 1) ++nparams; /* parametres de mes s'acumulen a l'ultim */
      }
      else /* qualsevol altre caracter acaba la sequencia */
      {
        state = ST_NORMAL;
        ++nparams; /* index -> comptador */
        esc_execute(c);
      }
      break;
    }
    default: break;
  }
}

int sys_write_console(char *buffer,int size)
{
  int i;

  for (i=0; i<size; i++)
    ansi_feed(buffer[i]);

  return size;
}

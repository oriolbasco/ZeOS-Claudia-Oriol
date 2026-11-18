/*
 * io.c - 
 */

#include <io.h>

#include <types.h>

#include <hardware.h>

/**************/
/** Screen  ***/
/**************/

#define NUM_COLUMNS 80
#define NUM_ROWS    25

Byte x, y=19;
Byte attr = 0x02; /* atribut VGA: fons<<4 | primer pla (verd sobre negre) */

void set_cursor(int col, int row)
{
  if (col < 0 || col >= NUM_COLUMNS || row < 0 || row >= NUM_ROWS) return;
  x = col;
  y = row;
}

void set_color(Byte fg, Byte bg)
{
  attr = ((bg & 0x7) << 4) | (fg & 0xF); /* bit 7 = blink, no el toquem */
}

void delete_char(void)
{
  if (x > 0) x--;
  else if (y > 0) 
  { 
    y--; x = NUM_COLUMNS - 1; 
  } else return;
  ((Word *)0xb8000)[y * NUM_COLUMNS + x] = (Word)' ' | (attr << 8);
}

void printc(char c)
{
  bochs_out(c);
  if (c=='\n')
  {
    x = 0;
    y=(y+1)%NUM_ROWS;
  }
  else
  {
    Word ch = (Word) (c & 0x00FF) | (attr << 8);
	Word *screen = (Word *)0xb8000;
	screen[(y * NUM_COLUMNS + x)] = ch;
    if (++x >= NUM_COLUMNS)
    {
      x = 0;
      y=(y+1)%NUM_ROWS;
    }
  }
}

void printc_xy(Byte mx, Byte my, char c)
{
  Byte cx, cy;
  cx=x;
  cy=y;
  x=mx;
  y=my;
  printc(c);
  x=cx;
  y=cy;
}

void printk(char *string)
{
  int i;
  for (i = 0; string[i]; i++)
    printc(string[i]);
}

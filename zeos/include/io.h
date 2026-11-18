/*
 * io.h - Definicio de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

void printc(char c);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);

void set_cursor(int col, int row);    /* 0-based; int perque el rang es comprovi abans de truncar */
void set_color(Byte fg, Byte bg);     /* colors VGA, no ANSI */
void delete_char(void);               /* backspace: retrocedeix i esborra */

#endif  /* __IO_H__ */

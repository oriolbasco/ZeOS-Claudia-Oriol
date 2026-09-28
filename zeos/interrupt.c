/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <libc.h>
//#include <stdio.h>

#include <zeos_interrupt.h>

Gate idt[IDT_ENTRIES];
Register    idtR;

char buff[256];

struct syst_stack {
  int edx;
  int ecx;
  int ebx;
  int esi;
  int edi;
  int ebp;
  int eax;
  int ds;
  int es;
  int fs;
  int gs;
  int error_code;
  int eip;
  int cs;
  int eflags;
  int esp;
  int ss;
};

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void keyboard_routine()
{
	unsigned char c_inp = inb(0x60);
	if ((c_inp & 0b10000000) == 0) 
	{
		unsigned char c_out = char_map[(c_inp & 0b01111111)];

		if (c_out < 'a' && c_out > '9') c_out = 'C';

		printc_xy(76,0,c_out);
	}
}

void clock_routine()
{
  zeos_show_clock();
}

//hay q imprimir mensaje + EIP(dirección de la instrucción que provocó el Page Fault) + cr2(dirección de memoria a la que esa instrucción intentó acceder) + CPU registers + while;
void my_page_fault_routine(struct syst_stack *contexto) {
    //contexto es un apuntador al tope de la pila
    
    printk("Process generates a PAGE FAULT exception at EIP: ");
    itoa_hex(contexto->eip, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    
    int num = read_cr2(); //func en assembly;
    printk("Offending address: ");
    itoa_hex(num, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    printk("CPU Registers: ");
    printk("\n");
    itoa_hex(contexto->edx, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    itoa_hex(contexto->ecx, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    itoa_hex(contexto->ebx, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    itoa_hex(contexto->esi, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    itoa_hex(contexto->edi, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    itoa_hex(contexto->esi, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    itoa_hex(contexto->ebp, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    
    itoa_hex(contexto->eax, buff); //guarda el entero en un buffer.
    printk(buff);
    printk("\n");
    while(1);
}

void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();
	
  setInterruptHandler(32, clock_handler, 0);	
  setInterruptHandler(33, keyboard_handler, 0);	
  
  //page fault excepcion:
  setInterruptHandler(14, my_page_fault_handler, 0);	
  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */

  set_idt_reg(&idtR);
}




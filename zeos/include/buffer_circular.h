#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define CBUF_CAPACITY 128

struct circular_buffer {
    char data[CBUF_CAPACITY];
    unsigned int head; //posicio de la proxima extracció
    unsigned int tail; //posicio on insertar la prixima tecla
    unsigned int count; //num caracters al buffer circular
};

void cbuf_init(struct circular_buffer *buf);

// Devuelven 1 si se realiza la operación, 0 si no es posible. 
int cbuf_push(struct circular_buffer *buf, char c);
int cbuf_pop(struct circular_buffer *buf, char *c);

#endif
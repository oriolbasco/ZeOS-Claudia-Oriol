#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define CBUF_SIZE 10

extern struct circular_buffer keyboard_buffer;

struct circular_buffer {
    char data[CBUF_SIZE];
    unsigned int head; // posicio de la proxima tecla que treurem
    unsigned int tail; // posicio on insertar la proxima tecla
    unsigned int count; // num caracters al buffer circular
};

void cbuf_init(struct circular_buffer *buf);

int cbuf_push(struct circular_buffer *buf, char c);
int cbuf_pop(struct circular_buffer *buf, char *c);

#endif
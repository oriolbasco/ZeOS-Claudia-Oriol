#include <buffer_circular.h>

void cbuf_init(struct circular_buffer *buf)
{
    buf->head = 0;
    buf->tail = 0;
    buf->count = 0;
}

//push de caracter al bufer circular
int cbuf_push(struct circular_buffer *buf, char c)
{
    if (buf->count == CBUF_SIZE) return 0;

    buf->data[buf->tail] = c;
    buf->tail = (buf->tail + 1) % CBUF_SIZE;
    buf->count++;

    return 1;
}

//guardem a c el primer caracter del bufer circular
int cbuf_pop(struct circular_buffer *buf, char *c)
{
    if (buf->count == 0) return 0;

    *c = buf->data[buf->head];
    buf->head = (buf->head + 1) % CBUF_SIZE;
    buf->count--;

    return 1;
}
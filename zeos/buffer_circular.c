#include <buffer_circular.h>

void cbuf_init(struct circular_buffer *buf)
{
    buf->head = 0;
    buf->tail = 0;
    buf->count = 0;
}

int cbuf_push(struct circular_buffer *buf, char c)
{
    if (buf->count == CBUF_CAPACITY) return 0;  //ple

    buf->data[buf->tail] = c;
    buf->tail = (buf->tail + 1) % CBUF_CAPACITY;
    buf->count++;

    return 1;
}

int cbuf_pop(struct circular_buffer *buf, char *c)
{
    if (buf->count == 0) return 0;  //esta buit

    *c = buf->data[buf->head];
    buf->head = (buf->head + 1) % CBUF_CAPACITY;
    buf->count--;

    return 1;
}
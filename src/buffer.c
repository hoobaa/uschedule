/*
 * reimplementation of Daniel Bernstein's buffer library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "buffer.h"

void
buffer_init (buffer * b, buffer_op op, int fd, char *buf,
			 unsigned int len)
{
	b->buf = buf;
	b->fd = fd;
	b->op = op;
	b->pos = 0;
	b->len = len;
}

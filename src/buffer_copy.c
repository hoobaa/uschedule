/*
 * reimplementation of Daniel Bernstein's buffer library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "buffer.h"

/* return  0: ok.
 * return -2: read error.
 * return -3: write error.
 * somewhat unusual ...
 */
int
buffer_copy (buffer * out, buffer * in)
{

	while (1) {
		int got;
		char *p;
		got = buffer_feed (in);
		if (got < 0)
			return -2;
		if (!got)
			break;
		p = buffer_PEEK (in);
		if (buffer_put (out, p, got) == -1)
			return -3;
		buffer_SEEK (in, got);
	}
	return 0;
}

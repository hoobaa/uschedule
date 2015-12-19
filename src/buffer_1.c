/*
 * reimplementation of Daniel Bernstein's buffer library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include "readwrite.h"
#include "buffer.h"

static char buffer_1_space[BUFFER_OUTSIZE];

static buffer buffer_1_buf =
       BUFFER_INIT ((buffer_op) write, 1, buffer_1_space, 
	                                        sizeof buffer_1_space);

buffer *buffer_1 = &buffer_1_buf;

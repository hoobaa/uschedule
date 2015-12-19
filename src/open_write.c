/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <sys/types.h>
#include <fcntl.h>
#include "open.h"

int open_write(const char *fname)
{ return open(fname,O_WRONLY | O_NDELAY); }
int open_write_blocking(const char *fname)
{ return open(fname,O_WRONLY); }


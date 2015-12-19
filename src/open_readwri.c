/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <sys/types.h>
#include <fcntl.h>
#include "open.h"
int open_readwrite_blocking(const char *fname)
  { return open(fname,O_RDWR); }
int open_readwrite(const char *fname)
  { return open(fname,O_RDWR|O_NDELAY); }

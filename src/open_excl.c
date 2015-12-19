/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <sys/types.h>
#include <fcntl.h>
#include "open.h"

int open_excl(const char *fname)
{ return open(fname,O_WRONLY | O_EXCL | O_CREAT |O_NDELAY,0644); }
int open_excl_mode(const char *fname,int mode)
{ return open(fname,O_WRONLY | O_EXCL | O_CREAT |O_NDELAY,mode); }

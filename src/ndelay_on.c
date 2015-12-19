/*
 * reimplementation of Daniel Bernstein's unix library.
 * placed in the public domain by Uwe Ohse, uwe@ohse.de.
 */
#include <sys/types.h>
#include <fcntl.h>
#include "ndelay.h"

/* love portability */
#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif

int
ndelay_on(int fd)
{
	int st;
	st=fcntl(fd,F_GETFL);
	if (st==-1)
		return -1;
	if (st & O_NONBLOCK)	
		return 0;
	return fcntl(fd,F_SETFL, st | O_NONBLOCK);
}
